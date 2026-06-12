#include "measurement.h"
#include "headers.h"

/* =========================================================
 * LOOKUP TABLE STRUCTURES
 * ========================================================= */

typedef struct {
  const char *name;
  enable_mux_number value;

} mux_map_t;

typedef struct {
  const char *name;
  vrcoil_number value;

} vrcoil_map_t;

typedef struct {
  const char *name;
  rcoil_number value;

} rcoil_map_t;

typedef struct {
  const char *name;
  usb_channel value;

} usb_map_t;

typedef struct {
  const char *name;
  uart_channel value;

} uart_map_t;

/* =========================================================
 * MUX TABLE
 * ========================================================= */

static const mux_map_t mux_table[] = {
    {"CON1_MUX1", CON1_MUX1}, {"CON1_MUX2", CON1_MUX2},
    {"CON1_MUX3", CON1_MUX3},

    {"CON2_MUX1", CON2_MUX1}, {"CON2_MUX2", CON2_MUX2},
    {"CON2_MUX3", CON2_MUX3}, {"CON2_MUX4", CON2_MUX4},

    {"GPIO_MUX", GPIO_MUX},

    {"CON3_MUX1", CON3_MUX1}, {"CON3_MUX2", CON3_MUX2},
    {"CON3_MUX3", CON3_MUX3}, {"CON3_MUX4", CON3_MUX4}};

/* =========================================================
 * VRCOIL TABLE
 * ========================================================= */

static const vrcoil_map_t vrcoil_table[] = {
    {"CON1_MUX1_SEL", CON1_MUX1_SEL},   {"CON1_MUX2_SEL", CON1_MUX2_SEL},
    {"CON1_MUX3_SEL", CON1_MUX3_SEL},

    {"CON2_MUX1_SEL", CON2_MUX1_SEL},   {"CON2_MUX2_SEL", CON2_MUX2_SEL},
    {"CON2_MUX3_SEL", CON2_MUX3_SEL},   {"CON2_MUX4_SEL", CON2_MUX4_SEL},

    {"VR_USB_SEL_DP", VR_USB_SEL_DP},   {"VR_USB_SEL_DN", VR_USB_SEL_DN},

    {"VR_UART_SEL_TX", VR_UART_SEL_TX}, {"VR_UART_SEL_RX", VR_UART_SEL_RX}};

/* =========================================================
 * RCOIL TABLE
 * ========================================================= */

static const rcoil_map_t rcoil_table[] = {{"CON3_MUX1_SEL", CON3_MUX1_SEL},
                                          {"CON3_MUX2_SEL", CON3_MUX2_SEL},
                                          {"CON3_MUX3_SEL", CON3_MUX3_SEL},
                                          {"CON3_MUX4_SEL", CON3_MUX4_SEL}};

/* =========================================================
 * USB TABLE
 * ========================================================= */
static const usb_map_t usb_table[] = {{"USB_CH0", USB_CH0},
                                      {"USB_CH1", USB_CH1},
                                      {"USB_CH2", USB_CH2},
                                      {"USB_CH3", USB_CH3}};
/* =========================================================
 * UART TABLE
 * ========================================================= */

static const uart_map_t uart_table[] = {{"UART_CH0", UART_CH0},
                                        {"UART_CH1", UART_CH1},
                                        {"UART_CH2", UART_CH2},
                                        {"UART_CH3", UART_CH3}};

static enable_mux_number string_to_mux(const char *str) {
  uint8_t i;

  for (i = 0; i < sizeof(mux_table) / sizeof(mux_table[0]); i++) {
    if (strcmp(str, mux_table[i].name) == 0) {
      return mux_table[i].value;
    }
  }

  return (enable_mux_number)255;
}

/* =========================================================
 * STRING -> PIN
 * ========================================================= */

static pin_number string_to_pin(const char *str) {
  uint8_t value = 0;

  if (strncmp(str, "PIN_", 4) != 0) {
    return (pin_number)255;
  }

  str += 4;
  while (*str) {
    if ((*str < '0') || (*str > '9')) {
      return (pin_number)255;
    }

    value = (value * 10) + (*str - '0');
    str++;
  }

  if (value > 15) {
    return (pin_number)255;
  }

  return (pin_number)value;
}

/* =========================================================
 * STRING -> VRCOIL
 * ========================================================= */

static vrcoil_number string_to_vrcoil(const char *str) {
  uint8_t i;

  for (i = 0; i < sizeof(vrcoil_table) / sizeof(vrcoil_table[0]); i++) {
    if (strcmp(str, vrcoil_table[i].name) == 0) {
      return vrcoil_table[i].value;
    }
  }

  return (vrcoil_number)255;
}

/* =========================================================
 * STRING -> RCOIL
 * ========================================================= */

static rcoil_number string_to_rcoil(const char *str) {
  uint8_t i;

  for (i = 0; i < sizeof(rcoil_table) / sizeof(rcoil_table[0]); i++) {
    if (strcmp(str, rcoil_table[i].name) == 0) {
      return rcoil_table[i].value;
    }
  }

  return (rcoil_number)255;
}

/* =========================================================
 * STRING -> USB
 * ========================================================= */

static usb_channel string_to_usb_channel(const char *str) {
  uint8_t i;

  for (i = 0; i < sizeof(usb_table) / sizeof(usb_table[0]); i++) {
    if (strcmp(str, usb_table[i].name) == 0) {
      return usb_table[i].value;
    }
  }

  return (usb_channel)255;
}
/* =========================================================
 * STRING -> UART
 * ========================================================= */

static uart_channel string_to_uart_channel(const char *str) {
  uint8_t i;

  for (i = 0; i < sizeof(uart_table) / sizeof(uart_table[0]); i++) {
    if (strcmp(str, uart_table[i].name) == 0) {
      return uart_table[i].value;
    }
  }

  return (uart_channel)255;
}
/* =========================================================
 * MAIN MEASUREMENT COMMAND HANDLER
 * ========================================================= */

void measurement_command_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  if (cmd->token_count < 3) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_MEASUREMENT\r\n",
                         sizeof("INVALID_MEASUREMENT\r\n") - 1);

    return;
  }

  /* ---------------------------------------- */
  /* VOLTAGE                                  */
  /* ---------------------------------------- */

  if (strcmp(cmd->tokens[1], "VOLT") == 0) {
    if (strcmp(cmd->tokens[2], "SINGLE") == 0) {
      voltage_single_handler(usb_data, cmd);

      return;
    }

    if (strcmp(cmd->tokens[2], "DIFF") == 0) {
      voltage_diff_handler(usb_data, cmd);

      return;
    }

    if (strcmp(cmd->tokens[2], "MULT") == 0) {
      voltage_multiple_handler(usb_data, cmd);

      return;
    }
  }

  /* ---------------------------------------- */
  /* RESISTANCE                               */
  /* ---------------------------------------- */

  if (strcmp(cmd->tokens[1], "RES") == 0) {
    resistance_handler(usb_data, cmd);

    return;
  }

  UART_Target_Transmit(usb_data->uartTargetIndex,
                       (uint8_t *)"INVALID_MEASUREMENT\r\n",
                       sizeof("INVALID_MEASUREMENT\r\n") - 1);
}

/* =========================================================
 * SINGLE VOLTAGE
 *
 * MES:VOLT:SINGLE:CON1_MUX1:PIN_5
 * ========================================================= */
void voltage_single_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  uint32_t voltage;

  enable_mux_number mux;
  pin_number pin;

  if (cmd->token_count != 5) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_VOLT_SINGLE\r\n",
                         sizeof("INVALID_VOLT_SINGLE\r\n") - 1);

    return;
  }

  mux = string_to_mux(cmd->tokens[3]);
  pin = string_to_pin(cmd->tokens[4]);

  if (((uint8_t)mux == 255) || ((uint8_t)pin == 255)) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_PARAMETER\r\n",
                         sizeof("INVALID_PARAMETER\r\n") - 1);

    return;
  }

  voltage = single_voltage_measurement(mux, pin);

  sprintf((char *)usb_data->tx_buffer, "VOLT:%lu\r\n", (unsigned long)voltage);

  UART_Target_Transmit(usb_data->uartTargetIndex, usb_data->tx_buffer,
                       strlen((char *)usb_data->tx_buffer));
}

/* =========================================================
 * DIFFERENTIAL VOLTAGE
 *
 * MES:VOLT:DIFF:CON1_MUX1:PIN_0:CON2_MUX1:PIN_1
 * ========================================================= */

void voltage_diff_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  uint32_t voltage;

  enable_mux_number mux1;
  enable_mux_number mux2;

  pin_number pin1;
  pin_number pin2;

  if (cmd->token_count != 7) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_VOLT_DIFF\r\n",
                         sizeof("INVALID_VOLT_DIFF\r\n") - 1);

    return;
  }

  mux1 = string_to_mux(cmd->tokens[3]);
  pin1 = string_to_pin(cmd->tokens[4]);

  mux2 = string_to_mux(cmd->tokens[5]);
  pin2 = string_to_pin(cmd->tokens[6]);

  if (((uint8_t)mux1 == 255) || ((uint8_t)mux2 == 255) ||
      ((uint8_t)pin1 == 255) || ((uint8_t)pin2 == 255)) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_PARAMETER\r\n",
                         sizeof("INVALID_PARAMETER\r\n") - 1);

    return;
  }

  voltage = differential_voltage_measurement(mux1, mux2, pin1, pin2);

  sprintf((char *)usb_data->tx_buffer, "DIFF:%lu\r\n", (unsigned long)voltage);

  UART_Target_Transmit(usb_data->uartTargetIndex, usb_data->tx_buffer,
                       strlen((char *)usb_data->tx_buffer));
}

/* =========================================================
 * MULTIPLE VOLTAGE
 *
 * MES:VOLT:MULT:PIN_3
 * ========================================================= */

void voltage_multiple_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  uint32_t value[16];

  pin_number pin;

  if (cmd->token_count != 4) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_VOLT_MULT\r\n",
                         sizeof("INVALID_VOLT_MULT\r\n") - 1);

    return;
  }

  pin = string_to_pin(cmd->tokens[3]);

  if ((uint8_t)pin == 255) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_PIN\r\n",
                         sizeof("INVALID_PIN\r\n") - 1);

    return;
  }

  multiple_voltage_measurement(pin, value);

  UART_Target_Transmit(usb_data->uartTargetIndex, (uint8_t *)"MULT_DONE\r\n",
                       sizeof("MULT_DONE\r\n") - 1);
}

/* =========================================================
 * RESISTANCE
 *
 * MES:RES:BET:CON1_MUX1_SEL:PIN_0:CON3_MUX1_SEL:PIN_1
 * ========================================================= */

void resistance_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  vrcoil_number vrcoil;
  rcoil_number rcoil;

  pin_number pin1;
  pin_number pin2;

  pin_number dummy = 0;

  usb_channel usb_ch;
  uart_channel uart_ch;

  /* Protect tokens[2] access */
  if (cmd->token_count < 3) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_RES_FORMAT\r\n",
                         sizeof("INVALID_RES_FORMAT\r\n") - 1);

    return;
  }

  /* =====================================================
   * MUX RESISTANCE
   * MES:RES:MUX:CON1_MUX1_SEL:PIN_0:CON3_MUX1_SEL:PIN_8
   * ===================================================== */
  if (strcmp(cmd->tokens[2], "MUX") == 0) {
    if (cmd->token_count != 7) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           (uint8_t *)"INVALID_RES_FORMAT\r\n",
                           sizeof("INVALID_RES_FORMAT\r\n") - 1);

      return;
    }

    vrcoil = string_to_vrcoil(cmd->tokens[3]);
    pin1 = string_to_pin(cmd->tokens[4]);

    rcoil = string_to_rcoil(cmd->tokens[5]);
    pin2 = string_to_pin(cmd->tokens[6]);

    if (((uint8_t)vrcoil == 255) || ((uint8_t)rcoil == 255) ||
        ((uint8_t)pin1 == 255) || ((uint8_t)pin2 == 255)) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           (uint8_t *)"INVALID_PARAMETER\r\n",
                           sizeof("INVALID_PARAMETER\r\n") - 1);

      return;
    }

    resistance_measurement(vrcoil, rcoil, pin1, pin2, USB_CH0, UART_CH0);
  }

  /* =====================================================
   * USB RESISTANCE
   * MES:RES:USB:VR_USB_SEL_DP:USB_CH2:CON3_MUX1_SEL:PIN_8
   * ===================================================== */
  else if (strcmp(cmd->tokens[2], "USB") == 0) {
    if (cmd->token_count != 7) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           (uint8_t *)"INVALID_RES_FORMAT\r\n",
                           sizeof("INVALID_RES_FORMAT\r\n") - 1);

      return;
    }

    vrcoil = string_to_vrcoil(cmd->tokens[3]);
    usb_ch = string_to_usb_channel(cmd->tokens[4]);

    rcoil = string_to_rcoil(cmd->tokens[5]);
    pin2 = string_to_pin(cmd->tokens[6]);

    if (((uint8_t)vrcoil == 255) || ((uint8_t)usb_ch == 255) ||
        ((uint8_t)rcoil == 255) || ((uint8_t)pin2 == 255)) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           (uint8_t *)"INVALID_PARAMETER\r\n",
                           sizeof("INVALID_PARAMETER\r\n") - 1);

      return;
    }

    resistance_measurement(vrcoil, rcoil, dummy, pin2, usb_ch, UART_CH0);
  }

  /* =====================================================
   * UART RESISTANCE
   * MES:RES:UART:VR_UART_SEL_RX:UART_CH3:CON3_MUX1_SEL:PIN_8
   * ===================================================== */
  else if (strcmp(cmd->tokens[2], "UART") == 0) {
    if (cmd->token_count != 7) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           (uint8_t *)"INVALID_RES_FORMAT\r\n",
                           sizeof("INVALID_RES_FORMAT\r\n") - 1);

      return;
    }

    vrcoil = string_to_vrcoil(cmd->tokens[3]);
    uart_ch = string_to_uart_channel(cmd->tokens[4]);

    rcoil = string_to_rcoil(cmd->tokens[5]);
    pin2 = string_to_pin(cmd->tokens[6]);

    if (((uint8_t)vrcoil == 255) || ((uint8_t)uart_ch == 255) ||
        ((uint8_t)rcoil == 255) || ((uint8_t)pin2 == 255)) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           (uint8_t *)"INVALID_PARAMETER\r\n",
                           sizeof("INVALID_PARAMETER\r\n") - 1);

      return;
    }

    /* FIX: pin1 was uninitialized */
    resistance_measurement(vrcoil, rcoil, dummy, pin2, USB_CH0, uart_ch);
  } else {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_RES_TYPE\r\n",
                         sizeof("INVALID_RES_TYPE\r\n") - 1);

    return;
  }

  UART_Target_Transmit(usb_data->uartTargetIndex, (uint8_t *)"RES_DONE\r\n",
                       sizeof("RES_DONE\r\n") - 1);
}