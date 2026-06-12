
#ifndef UART_CONTROLLER_H_
#define UART_CONTROLLER_H_

typedef enum {
    UART_CONTROLLER_IDLE=0,
    UART_CONTROLLER_SENDING,
    UART_CONTROLLER_WAITING,
    UART_CONTROLLER_RECEIVING,
} UART_Controller_State;

typedef struct {
    UART_Interface *uart;
    uint16_t numTx;
    uint16_t numRx;
    UART_Controller_State state;
} UART_Controller;


void UART_Controller_Transact(uint8_t controllerIndex, uint8_t *txData, uint16_t numTx, uint8_t *rxData, uint16_t numRx);
void UART_Controller_Initialize(UART_Interface *uart, uint8_t controllerIndex) ;
UART_Controller_State UART_Controller_getState(uint8_t targetIndex) ;

#endif