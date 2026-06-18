
#include <headers.h>

#define CURRENT_SOURCE_2_DAC 0x4A
#define CURRENT_SOURCE_2_IO  0x3E
#define ADS122C04            0x40

static int32_t adc_internal_offset = 0;
static differential_fsm_state_t fsm_current_tier = STATE_1UA;
static double i_value;

// --- Low-Level Hardware Drivers ---

static void io_expander_config(uint8_t slave_address) {
    uint8_t write_cmd[2] = {0x03, 0x00};
    i2c_write_verify(&Daughter_I2C, slave_address, write_cmd, 2);
}

void io_expander_daughter_reset() {
    uint8_t write_cmd[2] = {0x01, 0x00};
    i2c_write_verify(&Daughter_I2C, CURRENT_SOURCE_2_IO, write_cmd, 2);
}

void io_expander_daughter_config(void) {
    io_expander_config(CURRENT_SOURCE_2_IO);
}

void dac_set(double i_value_amps, current_value value) {
    uint8_t write_cmd[3] = {0x08, 0X6B, 0X6C};
    switch (value) {
        case current_1ua:    write_cmd[1]=0X6B; write_cmd[2]=0X6C; break;
        case current_10ua:   write_cmd[1]=0X6B; write_cmd[2]=0X6C; break;
        case current_100ua:  write_cmd[1]=0X67; write_cmd[2]=0X84; break;
        case current_1ma:    write_cmd[1]=0X67; write_cmd[2]=0X84; break;
        case current_10ma:   write_cmd[1]=0X6B; write_cmd[2]=0X1C; break;
        default:             write_cmd[1]=0xA8; write_cmd[2]=0xF5; break;
    }
    i2c_write_verify(&Daughter_I2C, CURRENT_SOURCE_2_DAC, write_cmd, 3);
}

static bool I2C_Wait_With_Timeout(I2C_Controller *controller) {
    uint32_t start_time = Delay_Timer_Get(&USB_BUFFER);
    uint32_t timeout_us = 500000;

    while (!I2C_Controller_Transaction_Done(controller)) {
        if ((Delay_Timer_Get(&USB_BUFFER) - start_time) >= timeout_us) {
            controller->state = I2C_CONTROLLER_IDLE;
            return false;
        }
    }
    return (controller->error == I2C_CONTROLLER_ERROR_NONE);
}

void current_source_2_select(current_value value) {
    uint8_t write_data[2] = {0x01, 0x00};
    switch (value) {
        case current_1ua:   write_data[1] = 0x03; break;
        case current_10ua:  write_data[1] = 0x07; break;
        case current_100ua: write_data[1] = 0x0B; break;
        case current_1ma:   write_data[1] = 0x0F; break;
        case current_10ma:  write_data[1] = 0x13; break;
        default: return;
    }
    i2c_write_verify(&Daughter_I2C, CURRENT_SOURCE_2_IO, write_data, 2);
}

// --- ADS122C04 ADC Drivers ---

void ads122c04_init(void) {
    uint8_t reset_cmd = 0x06;
    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &reset_cmd, 1);
    I2C_Wait_With_Timeout(&Daughter_I2C);
    delay_cycles(32000);

    // FIX: Configures Register 1 for External VREF (01) and Single-Shot Mode (CM = 0) -> Byte = 0x02
    uint8_t write_reg1[2] = {0x44, 0x02}; 
    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, write_reg1, 2);
    I2C_Wait_With_Timeout(&Daughter_I2C);
}

void ads122c04_read(uint8_t *out_data) {
    uint8_t cmd_start = 0x08;      // START/SYNC command
    uint8_t cmd_read_reg2 = 0x28;  // Read Register 2 command
    uint8_t reg2_value = 0;
    uint8_t cmd_rdata = 0x10;      // RDATA command

    // KICKSHOT: Send START command to trigger the single conversion
    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &cmd_start, 1);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return;

    delay_cycles(32000);  // Settle 1ms

    // Poll Configuration Register 2 until DRDY (bit 7) goes high
    do {
        I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &cmd_read_reg2, 1);
        if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return;
        delay_cycles(32000);

        I2C_Controller_Start_Receive(&Daughter_I2C, ADS122C04, &reg2_value, 1);
        if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return;
        delay_cycles(32000);

    } while ((reg2_value & 0x80) == 0);

    // Request conversion data payload
    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &cmd_rdata, 1);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return;
    delay_cycles(32000);

    I2C_Controller_Start_Receive(&Daughter_I2C, ADS122C04, out_data, 3);
    I2C_Wait_With_Timeout(&Daughter_I2C);
    delay_cycles(32000);
}

double ads122c04_raw_to_voltage(uint8_t *raw_bytes, uint8_t gain) {
    int32_t adc_code = ((int32_t)raw_bytes[0] << 16) | 
                       ((int32_t)raw_bytes[1] << 8)  | 
                        (int32_t)raw_bytes[2];

    // Signed 24-bit to 32-bit extension
    if (adc_code & 0x00800000) {
        adc_code |= 0xFF000000;
    }

    adc_code -= adc_internal_offset;
    
    // UNIPOLAR CLIPPING
    if (adc_code < 0) {
        adc_code = 0; 
    }

    // NOTE: Replace 5.0f with your actual measured external reference voltage if it isn't exactly 5.0V
    double v_ref = 5.0f; 
    double result=((double)adc_code / 8388608.0f) * (v_ref / (double)gain);
    return result;
}

// --- Calibration & Excitation Sets ---

double ads122c04_calibration(current_value value) {
    uint8_t write_cmd[2] = {0x40, 0x90}; // MUX = 1001 (AIN1-AVSS), Gain = 1
    uint8_t raw_adc_bytes[3];
   uint8_t gain=0;
    switch (value) {
        case current_1ua:
        case current_10ua:write_cmd[1] =0x90;gain=1; break;
        case current_100ua:write_cmd[1] = 0x92;gain=2; break; 
        case current_1ma:   write_cmd[1] = 0x90;gain=1; break; 
        case current_10ma:  write_cmd[1] = 0x90;gain=1; break; 
        default: return 0.0f;
    }

    // Short inputs configuration for internal offset calculation
    uint8_t short_inputs_cmd[2] = {0x40, (uint8_t)((write_cmd[1] & 0x0F) | 0xE0)}; 
    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, short_inputs_cmd, 2);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return 0.0f;
    delay_cycles(16000);

    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, write_cmd, 2);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return 0.0f;
    delay_cycles(32000);

    ads122c04_read(raw_adc_bytes);

    double measured_voltage = ads122c04_raw_to_voltage(raw_adc_bytes, gain);

    double calculated_current_amps = (measured_voltage) / 330.0f; 
    return calculated_current_amps;
}

double current_10ma_set(void) {
    i_value = 10e-3f; 
    dac_set(i_value, current_10ma);
   current_source_2_select(current_10ma);
     i_value=ads122c04_calibration(current_10ma);
    return i_value;
}

double current_1ma_set(void) {
    i_value = 1e-3f; 
   dac_set(i_value, current_1ma);
     current_source_2_select(current_1ma);
    i_value=ads122c04_calibration(current_1ma);
    return i_value;
}

double current_100ua_set(void) {
    i_value = 100e-6f; 
   dac_set(i_value, current_100ua);
   current_source_2_select(current_100ua);
     i_value= ads122c04_calibration(current_100ua);
    return i_value;
}

double current_10ua_set(void) {
    i_value = 10e-6f; 
    dac_set(i_value, current_10ua);
    current_source_2_select(current_10ua);   
    ads122c04_calibration(current_10ua);
    return i_value;
}

double current_1ua_set(void) {
    i_value = 1e-6f; 
    dac_set(i_value, current_1ua);
    current_source_2_select(current_1ua);
    ads122c04_calibration(current_1ua);
    return i_value;
}

double read_adc_differential(uint8_t gain) {
    uint8_t write_cmd[2] = {0x40, 0x80}; 
    uint8_t raw_adc_bytes[3];

    // Configures single-ended AIN0-AVSS (0x80) or with Gain 4 (0x84)
    // write_cmd[1] = (gain == 4) ? 0x84 : 0x80; 

    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, write_cmd, 2);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return 0.0f;

    delay_cycles(32000); 

    ads122c04_read(raw_adc_bytes);
    double measured_voltage=ads122c04_raw_to_voltage(raw_adc_bytes, gain);
    return measured_voltage ;
}

// --- Auto-Ranging FSM State Machine Engine ---

double run_differential_fsm_tick(void)
{
    double v_diff_unknown = 0.0f;
    double applied_current_amps = 0.0f; 
    uint8_t target_gain = 4;

    const double LOW_THRESHOLD  = 0.1f;
    const double HIGH_THRESHOLD = 4.25f;

    switch(fsm_current_tier)
    {
        case STATE_1UA:     applied_current_amps = current_1ua_set();   target_gain = 1; break;
        case STATE_10UA:    applied_current_amps = current_10ua_set();  target_gain = 1; break;
        case STATE_100UA:   applied_current_amps = current_100ua_set(); target_gain = 1; break;
        case STATE_1MA:     applied_current_amps = current_1ma_set();   target_gain = 1; break;
        case STATE_10MA:    applied_current_amps = current_10ma_set();  target_gain = 1; break;
        default: return 0.0f;
    }

    delay_cycles(1500000);  // 50ms settle delay

    v_diff_unknown = read_adc_differential(target_gain);

    if(v_diff_unknown <= 0.0f)
    {
        if(fsm_current_tier < STATE_10MA) { fsm_current_tier++; return -1.0f; }
        return 0.0f;
    }

    if(v_diff_unknown < LOW_THRESHOLD)
    {
        if(fsm_current_tier < STATE_10MA) { fsm_current_tier++; return -1.0f; }
    }

    if(v_diff_unknown > HIGH_THRESHOLD)
    {
        if(fsm_current_tier > STATE_1UA) { fsm_current_tier--; return -1.0f; }
    }

    if(applied_current_amps <= 0.0f)
    {
        return 0.0f;
    }

    double resistance = v_diff_unknown / applied_current_amps;
    return resistance;
}

void db_board_init(void) { 
    io_expander_daughter_config(); 
    ads122c04_init();
    io_expander_daughter_reset();
}

double get_single_shot_resistance(void)
{
    double resistance = -1.0f;
    fsm_current_tier = STATE_1UA;

    while(resistance < 0.0f)
    {
        resistance = run_differential_fsm_tick();
        if(resistance < 0.0f) { delay_cycles(320000); }
    }

    io_expander_daughter_reset();
    fsm_current_tier = STATE_1UA;
    return resistance;
}