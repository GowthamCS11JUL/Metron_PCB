
#ifndef UART_TARGET_H_
#define UART_TARGET_H_


typedef enum {
    UART_TARGET_IDLE=0,
    UART_TARGET_WAITING,
    UART_TARGET_RECEIVING,
    UART_TARGET_RECEIVED,
    UART_TARGET_SENDING,
} UART_Target_State;

typedef struct {
    UART_Interface *uart;
    Timer_Timeout *timer;
    uint32_t timeout;
    uint8_t *rxData;
    uint16_t numTx;
    uint16_t numRx;
    UART_Target_State state;
} UART_Target;

extern UART_Target glUartTarget[3];
void UART_Target_Initialize( UART_Interface *uart, uint8_t targetIndex, Timer_Timeout *timer, uint8_t timerInstance, uint32_t timeout) ;
void UART_Target_Receive(uint8_t targetIndex, uint8_t *rxData, uint16_t numRx);
void UART_Target_Transmit(uint8_t targetIndex, uint8_t *data, uint16_t numTx);
UART_Target_State UART_Target_getState(uint8_t targetIndex) ;
void UART_Target_setState(uint8_t targetIndex,UART_Target_State SET_STATE);

#endif