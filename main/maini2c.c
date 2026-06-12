#include "headers.h"

#define RELAY_DELAY_CYCLES (32000U)
/* =========================================================
 * IO EXPANDER CONFIG
 * ========================================================= */
static void io_expander_config(uint8_t slave_address) {
  uint8_t write_cmd[2] = {0x03, 0x00};

  i2c_write_verify(&Master_MCU, slave_address, write_cmd, 2);
}

/* =========================================================
 * IO EXPANDER INIT
 * ========================================================= */
void io_expander_init(void) {
  io_expander_config(U6);
  io_expander_config(U7);
  io_expander_config(U9);
  io_expander_config(U10);
  io_expander_config(U11);
  delay_cycles(RELAY_DELAY_CYCLES);
  reset_all();
}

/* =========================================================
 * RESET ALL OUTPUTS
 * ========================================================= */
void reset_all(void) {
  uint8_t write_cmd[2] = {0x01, 0x00};

  i2c_write_verify(&Master_MCU, U6, write_cmd, 2);
  i2c_write_verify(&Master_MCU, U7, write_cmd, 2);
  i2c_write_verify(&Master_MCU, U9, write_cmd, 2);
  i2c_write_verify(&Master_MCU, U10, write_cmd, 2);
  i2c_write_verify(&Master_MCU, U11, write_cmd, 2);
}

/* =========================================================
 * VRMUX SELECT
 * ========================================================= */
uint8_t enable_mux(enable_mux_number mux) {
  switch (mux) {
  case CON1_MUX1:
    return 0x01;
  case CON1_MUX2:
    return 0x02;
  case CON1_MUX3:
    return 0x04;
  case CON2_MUX1:
    return 0x08;
  case CON2_MUX2:
    return 0x10;
  case CON2_MUX3:
    return 0x20;
  case CON2_MUX4:
    return 0x40;
  case GPIO_MUX:
    return 0x80;
  case CON3_MUX1:
    return 0x01;
  case CON3_MUX2:
    return 0x02;
  case CON3_MUX3:
    return 0x04;
  case CON3_MUX4:
    return 0x08;
  default:
    return 0x00;
  }
}

/* =========================================================
 * VOLTAGE PIN SELECT
 * ========================================================= */
uint8_t vrpin_select(pin_number pin) { return (uint8_t)pin; }

/* =========================================================
 * RESISTOR PIN SELECT
 * ========================================================= */
uint8_t rpin_select(pin_number pin) { return ((uint8_t)pin << 4); }

/* =========================================================
 * MODE CHANGE TO RESISTOR A  COIL SELECT
 * ========================================================= */
uint8_t vrcoil_select(vrcoil_number num) {
  switch (num) {
  case CON1_MUX1_SEL:
    return 0x10;
  case CON1_MUX2_SEL:
    return 0x20;
  case CON1_MUX3_SEL:
    return 0x40;
  case CON2_MUX1_SEL:
    return 0x80;
  case CON2_MUX2_SEL:
    return 0x01;
  case CON2_MUX3_SEL:
    return 0x02;
  case CON2_MUX4_SEL:
    return 0x04;
  case VR_USB_SEL_DP:
    return 0x01;
  case VR_USB_SEL_DN:
    return 0x02;
  case VR_UART_SEL_TX:
    return 0x04;
  case VR_UART_SEL_RX:
    return 0x08;

  default:
    return 0x00;
  }
}

/* =========================================================
 * MODE CHANGE TO RESISTOR B  COIL SELECT
 * ========================================================= */
uint8_t rcoil_select(rcoil_number num) {
  switch (num) {
  case CON3_MUX1_SEL:
    return 0x08;
  case CON3_MUX2_SEL:
    return 0x10;
  case CON3_MUX3_SEL:
    return 0x20;
  case CON3_MUX4_SEL:
    return 0x40;
  default:
    return 0x00;
  }
}

/* =========================================================
 * SINGLE VOLTAGE MEASUREMENT
 * ========================================================= */
uint32_t single_voltage_measurement(enable_mux_number mux, pin_number pin) {
  reset_all();

  uint8_t mux_enable_cmd[2] = {0x01, 0x00};
  uint8_t mux_pin_cmd[2] = {0x01, 0x00};
  uint32_t value = 0x0000;
  switch (mux) {
  case CON1_MUX1:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(1);
    break;
  case CON1_MUX2:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(2);
    break;

  case CON1_MUX3:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(3);
    break;

  case GPIO_MUX:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(4);
    break;

  case CON2_MUX1:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(5);
    break;

  case CON2_MUX2:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(6);
    break;

  case CON2_MUX3:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(7);
    break;

  case CON2_MUX4:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = vrpin_select(pin);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(8);
    break;

  case CON3_MUX1:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = rpin_select(pin) | enable_mux(mux);
    i2c_write_verify(&Master_MCU, U11, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U11, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(9);
    break;

  case CON3_MUX2:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = rpin_select(pin) | enable_mux(mux);
    i2c_write_verify(&Master_MCU, U11, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U11, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(10);
    break;

  case CON3_MUX3:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = rpin_select(pin) | enable_mux(mux);
    i2c_write_verify(&Master_MCU, U11, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U11, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(11);
    break;
  case CON3_MUX4:
    mux_enable_cmd[1] = enable_mux(mux);
    mux_pin_cmd[1] = rpin_select(pin) | enable_mux(mux);
    i2c_write_verify(&Master_MCU, U11, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U11, mux_pin_cmd, 2);

    delay_cycles(RELAY_DELAY_CYCLES);
    value = AD4115_ReadSingleChannel(12);
    break;

  default:
    reset_all();
    value = 0xFFFFFFFF;
    break;
  }
  reset_all();
  delay_cycles(RELAY_DELAY_CYCLES);
  value=value* 10000U / 16777215U;
  return value;
}

/* =========================================================
 * DIFFERENTIAL VOLTAGE MEASUREMENT
 * ========================================================= */

uint32_t differential_voltage_measurement(enable_mux_number mux1,
                                          enable_mux_number mux2,
                                          pin_number pin1, pin_number pin2) {
  uint32_t value1 = single_voltage_measurement(mux1, pin1);
  uint32_t value2 = single_voltage_measurement(mux2, pin2);

  if ((value1 == 0xFFFFFFFFU) || (value2 == 0xFFFFFFFFU)) {
    return 0xFFFFFFFFU;
  }

  return (value1 >= value2) ? (value1 - value2) : (value2 - value1);
}

void multiple_voltage_measurement(pin_number pin, uint32_t value[16]) {
  value[0] = single_voltage_measurement(CON1_MUX1, pin);
  value[1] = single_voltage_measurement(CON1_MUX2, pin);
  value[2] = single_voltage_measurement(CON1_MUX3, pin);
  value[3] = single_voltage_measurement(GPIO_MUX, pin);

  value[4] = single_voltage_measurement(CON2_MUX1, pin);
  value[5] = single_voltage_measurement(CON2_MUX2, pin);
  value[6] = single_voltage_measurement(CON2_MUX3, pin);
  value[7] = single_voltage_measurement(CON2_MUX4, pin);

  value[8] = single_voltage_measurement(CON3_MUX1, pin);
  value[9] = single_voltage_measurement(CON3_MUX2, pin);
  value[10] = single_voltage_measurement(CON3_MUX3, pin);
  value[11] = single_voltage_measurement(CON3_MUX4, pin);

  value[12] = AD4115_ReadSingleChannel(13);
  value[13] = AD4115_ReadSingleChannel(14);
  value[14] = AD4115_ReadSingleChannel(15);

  value[15] = 0;
}

void resistance_measurement(vrcoil_number num1, rcoil_number num2,
                            pin_number pin1, pin_number pin2, usb_channel ch1,
                            uart_channel ch2) {
  reset_all();
  uint8_t relay_coil[2] = {0x01, 0x00};
  uint8_t mux_enable_cmd[2] = {0x01, 0x00};
  uint8_t pin_number[2] = {0x01, 0x00};
  uint8_t relay_coilu9[2] = {0x01, 0x00};

  switch (num1) {
  case CON1_MUX1_SEL:

    relay_coil[1] = vrcoil_select(num1);
    pin_number[1] = vrpin_select(pin1) | relay_coil[1];
    mux_enable_cmd[1] = enable_mux(CON1_MUX1);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case CON1_MUX2_SEL:

    relay_coil[1] = vrcoil_select(num1);
    pin_number[1] = vrpin_select(pin1) | relay_coil[1];
    mux_enable_cmd[1] = enable_mux(CON1_MUX2);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;

  case CON1_MUX3_SEL:
    relay_coil[1] = vrcoil_select(num1);
    pin_number[1] = vrpin_select(pin1) | relay_coil[1];
    mux_enable_cmd[1] = enable_mux(CON1_MUX3);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case CON2_MUX1_SEL:
    relay_coil[1] = vrcoil_select(num1);
    pin_number[1] = vrpin_select(pin1) | relay_coil[1];
    mux_enable_cmd[1] = enable_mux(CON2_MUX1);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;

  case CON2_MUX2_SEL:
    relay_coil[1] = vrcoil_select(num1);
    relay_coilu9[1] = relay_coil[1];
    pin_number[1] = vrpin_select(pin1);
    mux_enable_cmd[1] = enable_mux(CON2_MUX2);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;

  case CON2_MUX3_SEL:
    relay_coil[1] = vrcoil_select(num1);
    relay_coilu9[1] = relay_coil[1];
    pin_number[1] = vrpin_select(pin1);
    mux_enable_cmd[1] = enable_mux(CON2_MUX3);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    break;
  case CON2_MUX4_SEL:
    relay_coil[1] = vrcoil_select(num1);
    relay_coilu9[1] = relay_coil[1];
    pin_number[1] = vrpin_select(pin1);
    mux_enable_cmd[1] = enable_mux(CON2_MUX4);
    i2c_write_verify(&Master_MCU, U6, mux_enable_cmd, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U7, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;

  case VR_USB_SEL_DP:
    usb_resistance_test(ch1);
    relay_coil[1] = vrcoil_select(num1);
    i2c_write_verify(&Master_MCU, U10, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case VR_USB_SEL_DN:
    usb_resistance_test(ch1);
    relay_coil[1] = vrcoil_select(num1);
    i2c_write_verify(&Master_MCU, U10, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case VR_UART_SEL_TX:
    uart_rmux_select(ch2);
    relay_coil[1] = vrcoil_select(num1);
    i2c_write_verify(&Master_MCU, U10, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case VR_UART_SEL_RX:
    uart_rmux_select(ch2);
    relay_coil[1] = vrcoil_select(num1);
    i2c_write_verify(&Master_MCU, U10, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    break;
  default:
    return;
  }

  relay_coil[1] = 0;
  pin_number[1] = 0;
  mux_enable_cmd[1] = 0;

  switch (num2) {
  case CON3_MUX1_SEL:
    relay_coil[1] = rcoil_select(num2);
    relay_coil[1] = relay_coil[1] | relay_coilu9[1];
    pin_number[1] = rpin_select(pin2);
    mux_enable_cmd[1] = enable_mux(CON3_MUX1);
    pin_number[1] = pin_number[1] | mux_enable_cmd[1];

    i2c_write_verify(&Master_MCU, U11,pin_number , 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    break;
  case CON3_MUX2_SEL:
    relay_coil[1] = rcoil_select(num2);
    relay_coil[1] = relay_coil[1] | relay_coilu9[1];
    pin_number[1] = rpin_select(pin2);
    mux_enable_cmd[1] = enable_mux(CON3_MUX2);
    pin_number[1] = pin_number[1] | mux_enable_cmd[1];

    i2c_write_verify(&Master_MCU, U11, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case CON3_MUX3_SEL:
    relay_coil[1] = rcoil_select(num2);
    relay_coil[1] = relay_coil[1] | relay_coilu9[1];
    pin_number[1] = rpin_select(pin2);
    mux_enable_cmd[1] = enable_mux(CON3_MUX3);
    pin_number[1] = pin_number[1] | mux_enable_cmd[1];

    i2c_write_verify(&Master_MCU, U11, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    break;
  case CON3_MUX4_SEL:
    relay_coil[1] = rcoil_select(num2);
    relay_coil[1] = relay_coil[1] | relay_coilu9[1];
    pin_number[1] = rpin_select(pin2);
    mux_enable_cmd[1] = enable_mux(CON3_MUX4);
    pin_number[1] = pin_number[1] | mux_enable_cmd[1];

    i2c_write_verify(&Master_MCU, U11, pin_number, 2);
    delay_cycles(RELAY_DELAY_CYCLES);
    i2c_write_verify(&Master_MCU, U9, relay_coil, 2);
    delay_cycles(RELAY_DELAY_CYCLES);

    break;
  default:
    return;
  }
}