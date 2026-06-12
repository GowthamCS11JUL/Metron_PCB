#include "headers.h"



uint8_t result;
/* =========================================================
 * MAIN TEST COMMAND HANDLER
 * ========================================================= */

void test_command_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  if (cmd->token_count < 3) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_TEST\r\n",
                         sizeof("INVALID_TEST\r\n") - 1);

    return;
  }

  /* ---------------------------------------- */
  /* USB TEST                                 */
  /* ---------------------------------------- */

  if (strcmp(cmd->tokens[1], "USB") == 0) {
    usb_test_handler(usb_data, cmd);
    return;
  }

  /* ---------------------------------------- */
  /* UART TEST                                */
  /* ---------------------------------------- */

  if (strcmp(cmd->tokens[1], "UART") == 0) {
    uart_test_handler(usb_data, cmd);
    return;
  }
}

void usb_test_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  usb_channel ch;

  if (cmd->token_count != 3) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_USB_TEST\r\n",
                         sizeof("INVALID_USB_TEST\r\n") - 1);

    return;
  }

  if (strcmp(cmd->tokens[2], "CH0") == 0)
    ch = USB_CH0;
  else if (strcmp(cmd->tokens[2], "CH1") == 0)
    ch = USB_CH1;
  else if (strcmp(cmd->tokens[2], "CH2") == 0)
    ch = USB_CH2;
  else if (strcmp(cmd->tokens[2], "CH3") == 0)
    ch = USB_CH3;
  else {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_USB_CH\r\n",
                         sizeof("INVALID_USB_CH\r\n") - 1);

    return;
  }

  usb_function_test(ch);

  UART_Target_Transmit(usb_data->uartTargetIndex, (uint8_t *)"USB_TEST_OK\r\n",
                       sizeof("USB_TEST_OK\r\n") - 1);
}

void uart_test_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  uart_channel ch;

  if (cmd->token_count != 3) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_UART_TEST\r\n",
                         sizeof("INVALID_UART_TEST\r\n") - 1);

    return;
  }

  if (strcmp(cmd->tokens[2], "CH0") == 0)
    ch = UART_CH0;
  else if (strcmp(cmd->tokens[2], "CH1") == 0)
    ch = UART_CH1;
  else if (strcmp(cmd->tokens[2], "CH2") == 0)
    ch = UART_CH2;
  else if (strcmp(cmd->tokens[2], "CH3") == 0)
    ch = UART_CH3;
  else {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_UART_CH\r\n",
                         sizeof("INVALID_UART_CH\r\n") - 1);

    return;
  }

  result=uart_vmux_select(ch);
  if(result)
  {
  UART_Target_Transmit(usb_data->uartTargetIndex, (uint8_t *)"UART_TEST_PASS\r\n",
                       sizeof("UART_TEST_PASS\r\n") - 1);
  }
  else {
    UART_Target_Transmit(usb_data->uartTargetIndex, (uint8_t *)"UART_TEST_FAIL\r\n",
                       sizeof("UART_TEST_FAIL\r\n") - 1);
  }
}