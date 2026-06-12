// Target based on Timer
//-----------------------
/*
    Initially in UART_TARGET_IDLE state
    Start receive data from target - UART_TARGET_RECEIVING
    Based on a bus idle time, target will decide receive complete - UART_TARGET_RECEIVED
    Start sending data - UART_TARGET_SENDING
    Finish send all data - UART_TARGET_IDLE

    Target does not know how many bytes to receive. If the bus is idle for a specified time, target assumes recieve is complete. 
    Target know how many bytes to transfer. So TX is complete when all bytes are transferred. 

*/

#include <ti/driverlib/driverlib.h>
#include <UART_Interface.h>
#include <Timer_Timeout.h>
#include <UART_Target.h>

UART_Target glUartTarget[3];
uint8_t glUartTargetIndexFromTimerIndex[3];

void targetRxDoneCb(uint8_t instance, uint16_t numRx) {
    glUartTarget[instance].numRx = numRx;
    glUartTarget[instance].state = UART_TARGET_RECEIVED;
}
void targetTxDoneCb(uint8_t instance, uint16_t numTx) {
    glUartTarget[instance].numTx = numTx;
    glUartTarget[instance].state = UART_TARGET_IDLE;
}
void targetRxInProgressCb (uint8_t instance, uint16_t numRx) {
    glUartTarget[instance].numRx = numRx;
    if (glUartTarget[instance].timer->state == TIMER_TIMEOUT_IDLE)
        Timer_Timeout_Start(glUartTarget[instance].timer);
    else
        Timer_Timeout_reStart(glUartTarget[instance].timer);
    glUartTarget[instance].state = UART_TARGET_RECEIVING;
}
void targetTxInProgressCb(uint8_t instance, uint16_t numTx) {
    glUartTarget[instance].numTx = numTx;

}

void timerCallback(uint8_t instance) {
    UART_Target *trgt = &glUartTarget[glUartTargetIndexFromTimerIndex[instance]];
    UART_Interface_Stop_Receive(trgt->uart);
    trgt->state = UART_TARGET_RECEIVED;
}

void UART_Target_AssignCallBacks(uint8_t targetIndex) {
    glUartTarget[targetIndex].uart->rxDoneCallback = targetRxDoneCb;
    glUartTarget[targetIndex].uart->txDoneCallback = targetTxDoneCb;
    glUartTarget[targetIndex].uart->rxInProgressCallback = targetRxInProgressCb;
    glUartTarget[targetIndex].uart->txInProgressCallback = targetTxInProgressCb;
    glUartTarget[targetIndex].timer->callBack = timerCallback;
}

void UART_Target_Initialize( UART_Interface *uart, uint8_t targetIndex, Timer_Timeout *timer, uint8_t timerInstance, uint32_t timeout) {
    glUartTarget[targetIndex].uart = uart;
    glUartTarget[targetIndex].timer = timer;
    glUartTarget[targetIndex].timeout = timeout;
    glUartTarget[targetIndex].state = UART_TARGET_IDLE;
    glUartTargetIndexFromTimerIndex[timerInstance] = targetIndex;
    Timer_Timeout_Time_Set(timer, timeout);
    UART_Target_AssignCallBacks(targetIndex);
}

void UART_Target_Receive(uint8_t targetIndex, uint8_t *rxData, uint16_t numRx)
{
    UART_Target *ctrlr = &glUartTarget[targetIndex];
    UART_Interface_Start_Receive(ctrlr->uart, rxData, numRx);
    glUartTarget[targetIndex].state = UART_TARGET_WAITING;
}

void UART_Target_Transmit(uint8_t targetIndex, uint8_t *txData, uint16_t numTx)
{
    UART_Target *ctrlr = &glUartTarget[targetIndex];
    UART_Interface_Start_Transmit(ctrlr->uart, txData, numTx);
    ctrlr->state = UART_TARGET_SENDING;

}

UART_Target_State UART_Target_getState(uint8_t targetIndex) {
    return glUartTarget[targetIndex].state;
}

void UART_Target_setState(uint8_t targetIndex,UART_Target_State SET_STATE) {
    glUartTarget[targetIndex].state=SET_STATE;
}