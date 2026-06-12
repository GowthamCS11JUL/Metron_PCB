#include "usb.h"

#include "headers.h"

#define USB_CMD_TIMEOUT_US 1000000UL

/* ------------------------------------------------ */
/* INVALID RESPONSE                                 */
/* ------------------------------------------------ */

static const char invalid_cmd[] = "INVALID_COMMAND\r\n";

/* ------------------------------------------------ */
/* COMMAND POINTER                                  */
/* ------------------------------------------------ */

USB_command_map_t *executing_command;

/* ------------------------------------------------ */
/* COMMAND TABLE SETUP                              */
/* ------------------------------------------------ */

void usb_cmd_data_setup(UART_DATA *param, uint8_t index, const char *cmd,
                        const char *response, process_cmd_cb_t cb_fn) {
  uint8_t len;

  if (index >= MAX_NUM_CMDS) {
    return;
  }

  len = strlen(cmd);

  memcpy(param->command_table[index].command, cmd, len);

  param->command_table[index].command[len] = '\0';

  param->command_table[index].cmd_len = len;

  /* -------------------------------------------- */
  /* RESPONSE                                     */
  /* -------------------------------------------- */

  if (response != NULL) {
    uint8_t resp_len;
    resp_len = strlen(response);

    memcpy(param->command_table[index].response, response, resp_len);

    param->command_table[index].resp_len = resp_len;
  } else {
    param->command_table[index].resp_len = 0;
  }

  /* -------------------------------------------- */
  /* CALLBACK                                     */
  /* -------------------------------------------- */

  param->command_table[index].actions_func = cb_fn;
}

/* ------------------------------------------------ */
/* USB INIT                                         */
/* ------------------------------------------------ */

void usb_init(void) {
  memset(&USB_DATA, 0, sizeof(UART_DATA));

  /* -------------------------------------------- */
  /* UART INIT                                    */
  /* -------------------------------------------- */

  UART_Interface_Initialize(&USB_UART, UART0, 0, 255, 255);

  /* -------------------------------------------- */
  /* UART TARGET                                  */
  /* -------------------------------------------- */

  Timer_Timeout_Inialize(&USB_Timeout, TIMG0, 0, CPUCLK_FREQ);

  UART_Target_Initialize(&USB_UART, 0, &USB_Timeout, 0, 100000);

  /* -------------------------------------------- */
  /* DELAY TIMER                                  */
  /* -------------------------------------------- */

  Delay_Timer_Initialize(&USB_BUFFER, TIMG7, 7, CPUCLK_FREQ);

  Delay_Timer_Start(&USB_BUFFER);

  USB_DATA.uartTargetIndex = 0;
}

/* ------------------------------------------------ */
/* USB FSM                                          */
/* ------------------------------------------------ */

void usb_fsm(UART_DATA *param) {
  UART_Target_State state;

  state = UART_Target_getState(

      param->uartTargetIndex);

  switch (state) {
    /* ---------------------------------------- */
    /* IDLE                                     */
    /* ---------------------------------------- */

  case UART_TARGET_IDLE: {
    UART_Target_Receive(param->uartTargetIndex, &param->rx_byte, 1);

    break;
  }

    /* ---------------------------------------- */
    /* WAITING                                  */
    /* ---------------------------------------- */

  case UART_TARGET_WAITING: {
    break;
  }

    /* ---------------------------------------- */
    /* RECEIVING                                */
    /* ---------------------------------------- */

  case UART_TARGET_RECEIVING: {
    break;
  }

    /* ---------------------------------------- */
    /* RECEIVED                                 */
    /* ---------------------------------------- */

  case UART_TARGET_RECEIVED: {
    uint8_t byte;

    byte = param->rx_byte;

    /* ------------------------------------ */
    /* ARM NEXT BYTE                        */
    /* ------------------------------------ */

    UART_Target_Receive(param->uartTargetIndex, &param->rx_byte, 1);

    /* ------------------------------------ */
    /* FIRST BYTE                           */
    /* ------------------------------------ */

    if (param->command_active == 0) {
      param->command_active = 1;
      param->cmd_start_time = Delay_Timer_Get(&USB_BUFFER);
    }

    /* ------------------------------------ */
    /* RESTART TIMEOUT                      */
    /* ------------------------------------ */

    param->cmd_start_time = Delay_Timer_Get(&USB_BUFFER);

    /* ------------------------------------ */
    /* TERMINATION                          */
    /* ------------------------------------ */

    if ((byte == '\r') ||

        (byte == '\n')) {
      /*
       * Ignore empty CR/LF
       */

      if (param->cmd_index == 0) {
        break;
      }

      /*
       * Null terminate
       */

      param->cmd_buffer[param->cmd_index] = '\0';

      param->cmd_len = param->cmd_index;

      /*
       * Process command
       */
      process_usb_command(param);

      /*
       * Reset buffer
       */

      param->cmd_index = 0;

      param->command_active = 0;

      param->cmd_start_time = 0;

      memset(param->cmd_buffer, 0, sizeof(param->cmd_buffer));

    }

    /* ------------------------------------ */
    /* NORMAL DATA                          */
    /* ------------------------------------ */

    else {
      if (param->cmd_index < (MAX_CMD_LEN - 1)) {
        param->cmd_buffer[param->cmd_index++] = byte;
      } else {
        /*
         * BUFFER OVERFLOW
         */

        param->cmd_index = 0;

        param->command_active = 0;

        memset(param->cmd_buffer, 0, sizeof(param->cmd_buffer));

        UART_Target_Transmit(param->uartTargetIndex, (uint8_t *)invalid_cmd,
                             sizeof(invalid_cmd) - 1);
      }
    }

    break;
  }

    /* ---------------------------------------- */
    /* SENDING                                  */
    /* ---------------------------------------- */

  case UART_TARGET_SENDING: {
    break;
  }

  default: {
    break;
  }
  }

  /* ------------------------------------------------ */
  /* COMMAND TIMEOUT                                 */
  /* ------------------------------------------------ */
  if (param->command_active) {
    uint32_t now;

    now = Delay_Timer_Get(&USB_BUFFER);

    if ((now - param->cmd_start_time) >= USB_CMD_TIMEOUT_US) {
      if (param->cmd_index > 0) {
        param->cmd_buffer[param->cmd_index] = '\0';

        param->cmd_len = param->cmd_index;

        process_usb_command(param);
      }

      param->cmd_index = 0;

      param->command_active = 0;

      param->cmd_start_time = 0;

      memset(param->cmd_buffer, 0, sizeof(param->cmd_buffer));
    }
  }
}

/* ------------------------------------------------ */
/* VALIDATE FIXED COMMANDS                          */
/* ------------------------------------------------ */

USB_errors_t validate_usb_rx_data(UART_DATA *usb_data) {
  for (uint8_t i = 0; i < MAX_NUM_CMDS; i++) {
    if (usb_data->cmd_len == usb_data->command_table[i].cmd_len) {
      if (memcmp(usb_data->cmd_buffer, usb_data->command_table[i].command,
                 usb_data->cmd_len) == 0) {
        executing_command = &usb_data->command_table[i];

        return ERROR_NONE;
      }
    }
  }

  return INVALID_COMMAND;
}

/* ------------------------------------------------ */
/* PROCESS COMMAND                                  */
/* ------------------------------------------------ */

void process_usb_command(UART_DATA *usb_data) {
  Parsed_Command_t cmd;

  /* -------------------------------------------- */
  /* FIXED COMMANDS                               */
  /* -------------------------------------------- */

  if (validate_usb_rx_data(usb_data) == ERROR_NONE) {
    if (executing_command->actions_func != NULL) {
      executing_command->actions_func();
    }

    if (executing_command->resp_len > 0) {
      UART_Target_Transmit(usb_data->uartTargetIndex,
                           executing_command->response,
                           executing_command->resp_len);
    }

    return;
  }

  /* -------------------------------------------- */
  /* DYNAMIC COMMANDS                             */
  /* -------------------------------------------- */

  if (dynamic_parse((char *)usb_data->cmd_buffer, &cmd)) {

    command_dispatcher(usb_data, &cmd);
    return;
  }

  /* -------------------------------------------- */
  /* INVALID                                      */
  /* -------------------------------------------- */

  UART_Target_Transmit(usb_data->uartTargetIndex, (uint8_t *)invalid_cmd,
                       sizeof(invalid_cmd) - 1);
}
