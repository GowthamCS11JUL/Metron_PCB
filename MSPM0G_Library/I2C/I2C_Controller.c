

#include <ti/driverlib/driverlib.h>
#include <I2C_Controller.h>

void noActionController(uint8_t num)
{
    __NOP();
}
I2C_Controller_Errror I2C_Controller_Initialize(I2C_Controller *thisController, I2C_Regs *thisPort, uint8_t thisInstance)
{
    thisController->port = thisPort;
    thisController->instanceNum = thisInstance;
    thisController->state = I2C_CONTROLLER_IDLE;
    thisController->error = I2C_CONTROLLER_ERROR_NONE;

    thisController->tmpNumRx = 0;
    thisController->rxCallBack = noActionController;
    
    thisController->tmpNumTx = 0;
    thisController->txCallBack = noActionController;

    if (thisInstance == 0) {
        NVIC_ClearPendingIRQ(I2C0_INT_IRQn);
        NVIC_EnableIRQ(I2C0_INT_IRQn);
    }
    if (thisInstance == 1) {
        NVIC_ClearPendingIRQ(I2C1_INT_IRQn);
        NVIC_EnableIRQ(I2C1_INT_IRQn);
    }

    // Set up FIFO
    DL_I2C_setControllerTXFIFOThreshold(thisController->port, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(thisController->port, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    // Set up interrupt
    DL_I2C_enableInterrupt(thisController->port,
                           DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST |
                           DL_I2C_INTERRUPT_CONTROLLER_NACK |
                           DL_I2C_INTERRUPT_CONTROLLER_RXFIFO_TRIGGER |
                           DL_I2C_INTERRUPT_CONTROLLER_RX_DONE |
                           DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER |
                           DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

    return thisController->error;
}

I2C_Controller_Errror I2C_Controller_Start_Transmit(I2C_Controller *thisController, uint8_t thisTargetAddress, uint8_t *thisTxBuffer, uint16_t thisCount)
{
    thisController->numTx = thisCount;
    thisController->tmpNumTx = 0;
    thisController->txBuffer = thisTxBuffer;
    thisController->error = I2C_CONTROLLER_ERROR_NONE;
    thisController->state = I2C_CONTROLLER_BUSY;
    // Flush the TX FIFO
    DL_I2C_flushTargetTXFIFO(thisController->port);
    // Upload initial data to buffer
    thisController->tmpNumTx = DL_I2C_fillControllerTXFIFO(thisController->port, thisController->txBuffer, thisController->numTx);
    // Wait for the FIFO to be filled
    while (!(DL_I2C_getControllerStatus(thisController->port) & DL_I2C_CONTROLLER_STATUS_IDLE));
    // Start the transfer
    DL_I2C_startControllerTransfer(thisController->port, thisTargetAddress, DL_I2C_CONTROLLER_DIRECTION_TX, thisController->numTx);

    return thisController->error;

}


I2C_Controller_Errror I2C_Controller_Start_Receive(I2C_Controller *thisController, uint8_t thisTarget, uint8_t *thisBuffer, uint16_t thisNumRxBytes)
{
    thisController->numRx = thisNumRxBytes;
    thisController->tmpNumRx = 0;
    thisController->rxBuffer = thisBuffer;
    thisController->error = I2C_CONTROLLER_ERROR_NONE;
    thisController->state = I2C_CONTROLLER_BUSY;
    // Start the transfer
    DL_I2C_startControllerTransfer(thisController->port, thisTarget, DL_I2C_CONTROLLER_DIRECTION_RX, thisController->numRx);
    
    return thisController->error;
}


I2C_Controller_Errror getControllerStatus(I2C_Regs *thisPort)
{
     if (DL_I2C_getControllerStatus(thisPort) & DL_I2C_CONTROLLER_STATUS_ADDR_ACK )
         return I2C_CONTROLLER_ERROR_NACK;
     if (DL_I2C_getControllerStatus(thisPort) & DL_I2C_CONTROLLER_STATUS_DATA_ACK)
         return  I2C_CONTROLLER_ERROR_NACK;
     if (DL_I2C_getControllerStatus(thisPort) & DL_I2C_CONTROLLER_STATUS_ARBITRATION_LOST)
         return I2C_CONTROLLER_ERROR_DATA_ARBLOST;

     return I2C_CONTROLLER_ERROR_NONE;
}

void waitForControllerToIdle(I2C_Regs *thisPort)
{
    while (!(DL_I2C_getControllerStatus(thisPort) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (getControllerStatus(thisPort) != I2C_CONTROLLER_ERROR_NONE) {
            break;
        }
    }
}
bool I2C_Controller_Transaction_Done(I2C_Controller *thisController) {
    return (thisController->state == I2C_CONTROLLER_IDLE);
}

void I2C_Controller_IRQ (I2C_Controller *thisController)
{
    I2C_Regs *thisPort = thisController->port;
    switch (DL_I2C_getPendingInterrupt(thisPort)) {
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:
            while (DL_I2C_isControllerRXFIFOEmpty(thisPort) != true) {
                if (thisController->tmpNumRx <= thisController->numRx)
                    thisController->rxBuffer[thisController->tmpNumRx++] = DL_I2C_receiveControllerData(thisPort);
            }
            break;
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:
            if (thisController->tmpNumTx < thisController->numTx)
                thisController->tmpNumTx += DL_I2C_fillControllerTXFIFO(thisPort, &(thisController->txBuffer[thisController->tmpNumTx]), thisController->numTx - thisController->tmpNumTx);
            break;
        case DL_I2C_IIDX_CONTROLLER_RX_DONE:
            waitForControllerToIdle(thisPort);
            thisController->state = I2C_CONTROLLER_IDLE;
            thisController->rxCallBack(thisController->instanceNum);
            break;
        case DL_I2C_IIDX_CONTROLLER_TX_DONE:
            waitForControllerToIdle(thisPort);
            thisController->state = I2C_CONTROLLER_IDLE;
            thisController->txCallBack(thisController->instanceNum);
            break;            
        case DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST:
            thisController->error = I2C_CONTROLLER_ERROR_DATA_ARBLOST;
            thisController->state = I2C_CONTROLLER_IDLE;
            break;
        case DL_I2C_IIDX_CONTROLLER_NACK:
            thisController->error = I2C_CONTROLLER_ERROR_NACK;
            thisController->state = I2C_CONTROLLER_IDLE;
            break;
        default:
            break;
    }
}
