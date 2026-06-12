
// Controller
//-------------
/*
    Initially in UART_CONTROLLER_IDLE state
    Send data to target - UART_CONTROLLER_SENDING
    When all data send - UART_CONTROLLER_WAITING
    When target starts responding - UART_CONTROLLER_RECEIVING
    Received all data - Goeas back to UART_CONTROLLER_IDLE

    The controller need to know how many bytes to transmit and how many to receive from the target.
    If the expected number of bytes are not receieved, controller will not enter the UART_CONTROLLER_IDLE state.

*/

#include <ti/driverlib/driverlib.h>
#include <UART_Interface.h>
#include <UART_Controller.h>


UART_Controller glUartController[3];


void controllerRxDoneCb(uint8_t instance, uint16_t numRx) {
    glUartController[instance].numRx = numRx;    
    glUartController[instance].state = UART_CONTROLLER_IDLE;
}
void controllerTxDoneCb(uint8_t instance, uint16_t numTx) {
    glUartController[instance].numTx = numTx;
    glUartController[instance].state = UART_CONTROLLER_WAITING;
}
void controllerRxInProgressCb (uint8_t instance, uint16_t numRx) {
    glUartController[instance].numRx = numRx;
    glUartController[instance].state = UART_CONTROLLER_RECEIVING;

}
void controllerTxInProgressCb(uint8_t instance, uint16_t numTx) {
    glUartController[instance].numTx = numTx;
}

void UART_Controller_AssignCallBacks(uint8_t controllerIndex) {
    glUartController[controllerIndex].uart->rxDoneCallback = controllerRxDoneCb;
    glUartController[controllerIndex].uart->txDoneCallback = controllerTxDoneCb;
    glUartController[controllerIndex].uart->rxInProgressCallback = controllerRxInProgressCb;
    glUartController[controllerIndex].uart->txInProgressCallback = controllerTxInProgressCb;
}

void UART_Controller_Initialize(UART_Interface *uart, uint8_t controllerIndex) {
    glUartController[controllerIndex].uart = uart;
    glUartController[controllerIndex].state = UART_CONTROLLER_IDLE;
    UART_Controller_AssignCallBacks(controllerIndex);
}

void UART_Controller_Transact(uint8_t controllerIndex, uint8_t *txData, uint16_t numTx, uint8_t *rxData, uint16_t numRx)
{
    UART_Controller *ctrlr = &glUartController[controllerIndex];
    if (numRx > 0)
        UART_Interface_Start_Receive(ctrlr->uart, rxData, numRx);
    UART_Interface_Start_Transmit(ctrlr->uart, txData, numTx);
    ctrlr->state = UART_CONTROLLER_SENDING;
}

UART_Controller_State UART_Controller_getState(uint8_t targetIndex) {
    return glUartController[targetIndex].state;
}