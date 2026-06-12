#include "headers.h"
#include "ti/driverlib/dl_gpio.h"

#define UART_TX_STR(target, str)                                               \
  UART_Target_Transmit((target), (uint8_t *)(str), sizeof(str) - 1)

/* ------------------------------------------------ */
/* DEMO COMMAND HANDLER                             */
/* ------------------------------------------------ */

void demo_command_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  if (cmd->token_count < 4) {
    UART_TX_STR(usb_data->uartTargetIndex, "INVALID_DEMO_COMMAND\r\n");

    return;
  }

  if (strcmp(cmd->tokens[1], "LED") == 0) {
    led_demo_handler(usb_data, cmd);
    return;
  }

  UART_TX_STR(usb_data->uartTargetIndex, "INVALID_DEMO_COMMAND\r\n");
}

/* ------------------------------------------------ */
/* LED DEMO HANDLER                                */
/* ------------------------------------------------ */

void led_demo_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  /* -------------------------------------------- */
  /* GREEN LED                                    */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[2], "GREEN") == 0) {
    if (strcmp(cmd->tokens[3], "ON") == 0) {
      DL_GPIO_setPins(LED__PORT, LED__GREEN_PIN);

      UART_TX_STR(usb_data->uartTargetIndex, "DEMO:LED:GREEN:ON:OK\r\n");

      return;
    }

    if (strcmp(cmd->tokens[3], "OFF") == 0) {
      DL_GPIO_clearPins(LED__PORT, LED__GREEN_PIN);

      UART_TX_STR(usb_data->uartTargetIndex, "DEMO:LED:GREEN:OFF:OK\r\n");

      return;
    }
  }

  /* -------------------------------------------- */
  /* BLUE LED                                     */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[2], "BLUE") == 0) {
    if (strcmp(cmd->tokens[3], "ON") == 0) {
      DL_GPIO_setPins(LED__PORT, LED__BLUE_PIN);

      UART_TX_STR(usb_data->uartTargetIndex, "DEMO:LED:BLUE:ON:OK\r\n");

      return;
    }

    if (strcmp(cmd->tokens[3], "OFF") == 0) {
      DL_GPIO_clearPins(LED__PORT, LED__BLUE_PIN);

      UART_TX_STR(usb_data->uartTargetIndex, "DEMO:LED:BLUE:OFF:OK\r\n");

      return;
    }
  }

  /* -------------------------------------------- */
  /* RED LED                                      */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[2], "RED") == 0) {
    if (strcmp(cmd->tokens[3], "ON") == 0) {
      DL_GPIO_setPins(LED__PORT, LED__RED_PIN);

      UART_TX_STR(usb_data->uartTargetIndex, "DEMO:LED:RED:ON:OK\r\n");

      return;
    }

    if (strcmp(cmd->tokens[3], "OFF") == 0) {
      DL_GPIO_clearPins(LED__PORT, LED__RED_PIN);

      UART_TX_STR(usb_data->uartTargetIndex, "DEMO:LED:RED:OFF:OK\r\n");

      return;
    }
  }

  /* -------------------------------------------- */
  /* INVALID LED COMMAND                          */
  /* -------------------------------------------- */

  UART_TX_STR(usb_data->uartTargetIndex, "INVALID_LED_COMMAND\r\n");
}