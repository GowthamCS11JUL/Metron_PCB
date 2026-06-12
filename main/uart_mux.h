#ifndef UART_MUX_H_
#define UART_MUX_H_

typedef enum {
  UART_CH0 = 0,
  UART_CH1,
  UART_CH2,
  UART_CH3

} uart_channel;

void uart_pin_init(void);
uint8_t uart_vmux_select(uart_channel ch);
void uart_rmux_select(uart_channel ch);
uint8_t uart_loopback_start(void);

#endif