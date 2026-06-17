
// #include "headers.h"

// #define TARGET_VOLTAGE 0.5f
// #define SHORT_THRESHOLD 0.001f

// static const float current_table[] = {1e-6f, 10e-6f, 100e-6f, 1e-3f, 10e-3f};

// #define RELAY_DELAY_CYCLES (32000U)
// void dac_init(void) {
//   delay_cycles(RELAY_DELAY_CYCLES);
//   uint8_t write_cmd[3] = {0x08, 0x00, 0x0F};
//   i2c_write_verify(&Daughter_I2C, U4, pin_number, 3);
// }

// static void io_expander_config(uint8_t slave_address) {
//   uint8_t write_cmd[2] = {0x03, 0x00};

//   i2c_write_verify(&Daughter_I2C, slave_address, write_cmd, 2);
// }

// /* =========================================================
//  * IO EXPANDER INIT
//  * ========================================================= */
// void io_expander_init(void) {
//   io_expander_config(U2);
//   io_expander_config(U16);
//   delay_cycles(RELAY_DELAY_CYCLES);
//   reset_all();
// }

// /* =========================================================
//  * RESET ALL OUTPUTS
//  * ========================================================= */
// void reset_all(void) {
//   uint8_t write_cmd[2] = {0x01, 0x00};

//   i2c_write_verify(&Daughter_I2C, U2, write_cmd, 2);
//   i2c_write_verify(&Daughter_I2C, U16, write_cmd, 2);
// }

// void adc_init(void) {
//   uint8_t write_cmd = 0x06; // RESET Command

//   // Passed using '&' to guarantee the driver receives a proper memory
//   address
//   // pointer
//   I2C_Controller_Start_Transmit(&Daughter_I2C, Andc, &write_cmd, 1);
//   delay_cycles(3200);

//   adc_config();
// }
// void adc_config(void) {
//   uint8_t tx_buffer[2] = {0x44,
//                           0x02}; // 0x44 (WREG Reg 1) | 0x02 (Config options)
//   I2C_Controller_Start_Transmit(&Daughter_I2C, Andc, tx_buffer, 2);
//   delay_cycles(3200);
// }
// void adc_1_set(void) {
//   uint8_t tx_buffer[2] = {
//       0x40, 0x00}; // 0x40 (WREG Reg 0) | 0x00 (MUX: AIN0-AIN1, Gain: 1)
//   I2C_Controller_Start_Transmit(&Daughter_I2C, Andc, tx_buffer, 2);
//   delay_cycles(3200);
// }

// uint8_t adc_is_data_ready(void) {
//   uint8_t rreg_cmd =
//       0x28; // RREG command targeting Register 2 (0x20 | (0x02 << 2))
//   uint8_t reg_value = 0;

//   I2C_Controller_Start_Transmit(&Daughter_I2C, Andc, &rreg_cmd, 1);
//   I2C_Controller_Start_Receive(&Daughter_I2C, Andc, &reg_value, 1);

//   // Bit 7 (DRDY bit) transitions to 1 when a new conversion is ready
//   if (reg_value & 0x80) {
//     return 1;
//   }
//   return 0;
// }

// void adc_trigger_conversion(void) {
//   uint8_t start_cmd = 0x08; // START/SYNC Command
//   I2C_Controller_Start_Transmit(&Daughter_I2C, Andc, &start_cmd, 1);
// }

// float read_resistance_voltage(void) {
//   adc_1_set();              // Shift MUX targeting to AIN0/AIN1
//   adc_trigger_conversion(); // Kick off the hardware single-shot sample

//   int32_t latest_voltage =
//       adc_get_conversion_result(); // Wait and capture conversion data

//   // Protect against small negative noise values
//   if (latest_voltage < 0) {
//     latest_voltage = 0;
//   }
//  return ((float)latest_voltage * 10.0f) / 8388607.0f;
// }

// void current_set_mode2(current_value value) {
//   uint8_t write_cmd[2] = {0x01, 0x00};
//   switch (value) {
//   case one_microamp:
//     write_cmd[1] = 0x03;
//     break;
//   case ten_microamp:
//     write_cmd[1] = 0x07;
//     break;
//   case hundred_microamp:
//     write_cmd[1] = 0x0B;
//     break;
//   case one_milli_amp:
//     write_cmd[1] = 0x0F;
//     break;
//   case ten_milli_amp:
//     write_cmd[1] = 0x13;
//     break;
//   default:
//     write_cmd[1] = 0x13;

//     break;
//   }
//   i2c_write_verify(&Daughter_I2C, U2, write_cmd, 2);
// }
// void resistance_fsm_init(resistance_fsm_t *fsm)
// {
//     fsm->state = RES_SET_CURRENT;

//     fsm->range = one_microamp;

//     fsm->voltage = 0;

//     fsm->resistance = 0;

//     fsm->complete = false;
// }
// void resistance_fsm_task(resistance_fsm_t *fsm) {
//   switch (fsm->state) {
//   case RES_SET_CURRENT: {
//     current_set_mode2(fsm->range);

//     /* Start settle timer here */

//     fsm->state = RES_WAIT_SETTLE;
//     break;
//   }

//   case RES_WAIT_SETTLE: {
//     delay_cycles(32000);
//     fsm->state = RES_MEASURE;

//     break;
//   }

//   case RES_MEASURE: {
//     fsm->voltage = read_resistance_voltage();

//     if (fsm->voltage >= TARGET_VOLTAGE) {
//       fsm->state = RES_CALCULATE;
//     } else {
//       if (fsm->range < ten_milli_amp) {
//         fsm->range++;
//         fsm->state = RES_SET_CURRENT;
//       } else {
//         if (fsm->voltage > SHORT_THRESHOLD) {
//           fsm->state = RES_CALCULATE;
//         } else {
//           fsm->state = RES_SHORT_CIRCUIT;
//         }
//       }
//     }

//     break;
//   }

//   case RES_CALCULATE: {
//     fsm->resistance = fsm->voltage / current_table[fsm->range];

//     fsm->state = RES_DONE;
//     break;
//   }

//   case RES_SHORT_CIRCUIT: {
//     fsm->resistance = 0.0f;

//     fsm->state = RES_DONE;
//     break;
//   }

//   case RES_DONE: {
//     fsm->complete = true;
//     break;
//   }

//   default:
//     break;
//   }
// }

// uint32_t resistance_measured()
// {
// resistance_fsm_t ResistanceFSM;

// resistance_fsm_init(&ResistanceFSM);

//  while(!fsm.complete)
//     {
//         resistance_fsm_task(&fsm);
//     }
// float resistance;

// resistance = measure_resistance();

// return (uint32_t)resistance*1000000;
// }


#include <headers.h>

#define CURRENT_SOURCE_2_DAC 0x4A
#define CURRENT_SOURCE_2_IO 0X3E
#define ADS122C04 0x40

static void io_expander_config(uint8_t slave_address) {
  uint8_t write_cmd[2] = {0x03, 0x00};

  i2c_write_verify(&Daughter_I2C, slave_address, write_cmd, 2);
}

void io_expander_daughter_config(void) {
  io_expander_config(CURRENT_SOURCE_2_IO);
}
void dac_set(void) {
  uint8_t write_cmd[3] = {0x08, 0xA8, 0xF5};
  i2c_write_verify(&Daughter_I2C, CURRENT_SOURCE_2_DAC, write_cmd, 3);
}

void db_board_init(void) 
{ 
    io_expander_daughter_config(); 
    ads122c04_init();
}


static bool I2C_Wait_With_Timeout(I2C_Controller *controller) {
  uint32_t start_time;
  uint32_t timeout_us = 500000;

  start_time = Delay_Timer_Get(&USB_BUFFER);

  while (!I2C_Controller_Transaction_Done(controller)) {
    if ((Delay_Timer_Get(&USB_BUFFER) - start_time) >= timeout_us) {
      controller->state = I2C_CONTROLLER_IDLE;
      return false;
    }
  }

  if (controller->error != I2C_CONTROLLER_ERROR_NONE) {
    return false;
  }

  return true;
}

void current_source_2_select(current_value value) {
  uint8_t write_data[2] = {0x01, 0x00};
  switch (value) {
  case current_1ua:
    write_data[1] = 0x03;
    break;
  case current_10ua:
    write_data[1] = 0x07;
    break;
  case current_100ua:
    write_data[1] = 0x0B;
    break;
  case current_1ma:
    write_data[1] = 0x0F;
    break;
  case current_10ma:
    write_data[1] = 0x13;
    break;
  default:
    break;
  }
  i2c_write_verify(&Daughter_I2C, CURRENT_SOURCE_2_IO, write_data, 2);
}

void ads122c04_init(void) {
  uint8_t reset_cmd = 0x06;
  I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &reset_cmd, 1);
  I2C_Wait_With_Timeout(&Daughter_I2C);
  delay_cycles(32000);

  // Set Register 1 to 0x02 (External VREF, Continuous or Single-Shot depending on your setup)
  uint8_t write_reg1[2] = {0x44, 0x02}; 
  I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, write_reg1, 2);
  I2C_Wait_With_Timeout(&Daughter_I2C);
}

void start_sync_cmd(void) {
  uint8_t start_cmd = 0x08;
  I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &start_cmd, 1);
  if (!I2C_Wait_With_Timeout(&Daughter_I2C))
    return;
}

// Pass the 3-byte destination array into the function to avoid stack corruption
void ads122c04_read(uint8_t *out_data)
 {
  uint8_t cmd_start = 0x08;
  uint8_t cmd_read_reg2 = 0x28;
  uint8_t reg2_value = 0;
  uint8_t cmd_rdata = 0x10;

  // 1. Kick off a Single-Shot conversion
  I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &cmd_start, 1);
  if (!I2C_Wait_With_Timeout(&Daughter_I2C))
    return;

  // Give the hardware a tiny bit of breathing room before polling
  delay_cycles(1000);

  // 2. Poll Register 2 until Bit 7 (DRDY) goes HIGH
  do {
    // Send RREG command for Register 2
    I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &cmd_read_reg2, 1);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C))
      return;
    // Wait briefly if your I2C driver requires blocking time, then read back 1
    // byte
    I2C_Controller_Start_Receive(&Daughter_I2C, ADS122C04, &reg2_value, 1);
    if (!I2C_Wait_With_Timeout(&Daughter_I2C))
      return;

    // Add a small delay to prevent pinning the I2C bus with infinite
    // back-to-back spam
    delay_cycles(2000);

  } while ((reg2_value & 0x80) == 0);

  // 3. DRDY is 1! Send the RDATA command
  I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, &cmd_rdata, 1);
  if (!I2C_Wait_With_Timeout(&Daughter_I2C))
    return;

  // 4. Read back the 3 bytes directly into your output destination
  I2C_Controller_Start_Receive(&Daughter_I2C, ADS122C04, out_data, 3);
  if (!I2C_Wait_With_Timeout(&Daughter_I2C))
    return;
}
float ads122c04_raw_to_voltage(uint8_t *raw_bytes, uint8_t gain) 
{
    // 1. Combine 3 bytes into a signed 32-bit integer (Big-Endian format)
    int32_t adc_code = ((int32_t)raw_bytes[0] << 16) | 
                       ((int32_t)raw_bytes[1] << 8)  | 
                        (int32_t)raw_bytes[2];

    // 2. Sign-extend the 24-bit value into a proper 32-bit two's complement signed integer
    if (adc_code & 0x00800000) {
        adc_code |= 0xFF000000;
    }

    // 3. Apply the voltage conversion formula (Vref = 5.0V)
    float v_ref = 5.0f;
    float voltage = ((float)adc_code / 8388608.0f) * (v_ref / (float)gain);

    return voltage;
}

void ads122c04_calibration(current_value value) {
  // Command byte 0x40 targets Register 0
  uint8_t write_cmd[2] = {0x40, 0x00}; 
  uint8_t raw_adc_bytes[3];

  switch (value) {
    case current_1ua:
    case current_10ua:
    case current_100ua:
    case current_1ma:
      // MUX = AIN1-AVSS (0x90), Gain = 4 (0x04), Bypassed PGA (0x01) -> 0x95
      write_cmd[1] = 0x95; 
      break;

    case current_10ma:
      // MUX = AIN1-AVSS (0x90), Gain = 1 (0x00), Bypassed PGA (0x01) -> 0x91
      write_cmd[1] = 0x91; 
      break;

    default:
      break;
  }

  // 1. Send the updated MUX + Gain combination to Register 0
  I2C_Controller_Start_Transmit(&Daughter_I2C, ADS122C04, write_cmd, 2);
  if (!I2C_Wait_With_Timeout(&Daughter_I2C)) return;

  delay_cycles(32000); // Give the analog front-end settling time

  // 2. Read the fresh conversion data
  ads122c04_read(raw_adc_bytes);
}
void current_10ma_set() {
  current_source_2_select(current_10ma);
  ads122c04_calibration(current_10ma);
  dac_set();
}

void current_1ma_set() {
  current_source_2_select(current_1ma);
  ads122c04_calibration(current_1ma);
   dac_set();
}

void current_100ua_set() {
  current_source_2_select(current_100ua);
  ads122c04_calibration(current_100ua);
   dac_set();
}

void current_10ua_set() {
  current_source_2_select(current_10ua);
  ads122c04_calibration(current_10ua);
   dac_set();
}

void current_1ua_set() {
  current_source_2_select(current_1ua);
  ads122c04_calibration(current_1ua);
   dac_set();
}
