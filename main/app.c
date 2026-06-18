#include "headers.h"
#include "variables.h"

void app_init() {
  usb_init();
  cmd_data_setup();
  AD4115BCPZ_RL7_init(&ADC);
  i2c0_init();
  i2c1_init();
  io_expander_init();
  usb_pins_init();
  uart_pin_init();
  db_board_init();
}

void cmd_data_setup(void) {
    
}



void command_dispatcher(UART_DATA *usb_data, Parsed_Command_t *cmd) {
  if (cmd->token_count == 0) {
    UART_Target_Transmit(usb_data->uartTargetIndex,
                         (uint8_t *)"INVALID_COMMAND\r\n",
                         sizeof("INVALID_COMMAND\r\n") - 1);

    return;
  }

  /* -------------------------------------------- */
  /* MES                                          */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[0], "MES") == 0) {
    measurement_command_handler(usb_data, cmd);
    return;
  }

  /* -------------------------------------------- */
  /* TEST                                         */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[0], "TEST") == 0) {
    test_command_handler(usb_data, cmd);
    return;
  }

  /* -------------------------------------------- */
  /* DEMO                                         */
  /* -------------------------------------------- */

  if (strcmp(cmd->tokens[0], "DEMO") == 0) {
    demo_command_handler(usb_data, cmd);
    return;
  }

  /* -------------------------------------------- */
  /* UNKNOWN                                      */
  /* -------------------------------------------- */

  UART_Target_Transmit(usb_data->uartTargetIndex,
                       (uint8_t *)"UNKNOWN_COMMAND\r\n",
                       sizeof("UNKNOWN_COMMAND\r\n") - 1);
}
void UART0_IRQHandler(void) { UART_Interface_IRQ(&USB_UART); }

void TIMG0_IRQHandler(void) { Timer_Timeout_IRQ(&USB_Timeout); }

void TIMG7_IRQHandler(void) { Delay_Timer_IRQ(&USB_BUFFER); }

void I2C0_IRQHandler(void) { I2C_Controller_IRQ(&Daughter_I2C); }

void I2C1_IRQHandler(void) { I2C_Controller_IRQ(&Master_MCU); }

void UART3_IRQHandler(void) { UART_Interface_IRQ(&UART_MUX); }

void TIMG6_IRQHandler(void) { Timer_Timeout_IRQ(&UART_MUX_Timeout); }

void SPI_0_INST_IRQHandler(void) { SPI_IRQ(&ADC); }