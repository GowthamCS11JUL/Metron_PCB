#ifndef VARIABLES_H_
#define VARIABLES_H_

UART_Interface USB_UART;
Timer_Timeout USB_Timeout;
UART_DATA USB_DATA;
USB_command_map_t *executing_command;
UART_Interface UART_MUX;
Timer_Timeout UART_MUX_Timeout;
Delay_Timer USB_BUFFER;
I2C_Controller Daughter_I2C;
I2C_Controller Master_MCU;
SPI_Block_Interface ADC;
#endif