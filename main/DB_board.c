
// #include "DB_board.h"
// #include "header.h"
// #include "ti/driverlib/m0p/dl_core.h"
// #include <cstdint>

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

//   // Passed using '&' to guarantee the driver receives a proper memory address
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