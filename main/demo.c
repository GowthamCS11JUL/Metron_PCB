#include "headers.h"

#define UART_TX_STR(target, str)                                               \
  UART_Target_Transmit((target), (uint8_t *)(str), sizeof(str) - 1)

/* ------------------------------------------------ */
/* DEMO COMMAND HANDLER                             */
/* ------------------------------------------------ */

void demo_command_handler(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  if (cmd->token_count < 2) {
    UART_TX_STR(usb_data->uartTargetIndex, "INVALID_DEMO_COMMAND\r\n");
    return;
  }

  /* -------------------------------------------- */
  /* DEMO:RES                                     */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[1], "RES") == 0) {
    resistance_demo_handler(usb_data, cmd);
    return;
  }

  UART_TX_STR(usb_data->uartTargetIndex, "INVALID_DEMO_COMMAND\r\n");
}

/* ------------------------------------------------ */
/* RESISTANCE DEMO HANDLER                          */
/* ------------------------------------------------ */

void resistance_demo_handler(UART_DATA *usb_data, Parsed_Command_t *cmd)
{
    float resistance;

    resistance = get_single_shot_resistance() - 330.0f;

    if(resistance <= 0)
    {
        resistance = 0;
    }

    if(resistance >= 4900000.0f)
    {
        sprintf((char *)usb_data->tx_buffer,
                "DEMO:RES:OL\r\n");
    }
    else
    {
        sprintf((char *)usb_data->tx_buffer,
                "DEMO:RES:%.2f OHMS\r\n",
                resistance);
    }

    UART_Target_Transmit(usb_data->uartTargetIndex,
                         usb_data->tx_buffer,
                         strlen((char *)usb_data->tx_buffer));
}