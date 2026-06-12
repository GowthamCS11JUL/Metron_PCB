
#include <ti/driverlib/driverlib.h>
#include <I2C_Target.h>

void noActionTarget(uint8_t num)
{
    __NOP();
}
void I2C_Target_Initialize(I2C_Target *thisTarget, I2C_Regs *thisPort, uint8_t thisInstance, uint8_t thisAddress)
{
    thisTarget->port = thisPort;
    thisTarget->address = thisAddress;
    thisTarget->instanceNum = thisInstance;  
    thisTarget->error = I2C_TARGET_ERROR_NONE;

    thisTarget->tmpNumRx = 0;
    thisTarget->rxState = I2C_TARGET_IDLE;
    thisTarget->rxCallBack = noActionTarget;

    thisTarget->tmpNumTx = 0;
    thisTarget->txState = I2C_TARGET_IDLE;
    thisTarget->txCallBack = noActionTarget;

    DL_I2C_flushTargetTXFIFO(thisTarget->port);
    if (thisInstance == 0)
    {
        NVIC_ClearPendingIRQ(I2C0_INT_IRQn);
        NVIC_EnableIRQ(I2C0_INT_IRQn);
    }
    if (thisInstance == 1)
    {
        NVIC_ClearPendingIRQ(I2C1_INT_IRQn);
        NVIC_EnableIRQ(I2C1_INT_IRQn);
    }

        /* Configure Target Mode */
    DL_I2C_setTargetOwnAddress(thisTarget->port, thisTarget->address);
    DL_I2C_setTargetTXFIFOThreshold(thisTarget->port, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setTargetRXFIFOThreshold(thisTarget->port, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    /* Configure Interrupts */
    DL_I2C_enableInterrupt(thisTarget->port,
                           DL_I2C_INTERRUPT_TARGET_RXFIFO_TRIGGER |
                           DL_I2C_INTERRUPT_TARGET_START |
                           DL_I2C_INTERRUPT_TARGET_STOP |
                           DL_I2C_INTERRUPT_TARGET_TXFIFO_TRIGGER);
}

I2C_Target_Errror I2C_Target_Setup_Transmit(I2C_Target *thisTarget, uint8_t *data, uint16_t numMaxTx)
{
    if (thisTarget->txState != I2C_TARGET_IDLE){
        thisTarget->error = I2C_TARGET_ERROR_BUSY;
        return thisTarget->error;
    }
    thisTarget->numMaxTx = numMaxTx;
    thisTarget->tmpNumTx = 0;    
    thisTarget->txBuffer = data;
    thisTarget->txState = I2C_TARGET_STARTED;
    thisTarget->error = I2C_TARGET_ERROR_NONE;
    DL_I2C_flushTargetTXFIFO(thisTarget->port);

    // Fill one byte from the target register location to get the TX started
   thisTarget->tmpNumTx  = DL_I2C_fillTargetTXFIFO(thisTarget->port, thisTarget->txBuffer, 1);

    return I2C_TARGET_ERROR_NONE;
}

I2C_Target_Errror I2C_Target_Setup_Recieve(I2C_Target *thisTarget, uint8_t *data, uint16_t thisMaxRx)
{
    if (thisTarget->rxState != I2C_TARGET_IDLE){
        thisTarget->error = I2C_TARGET_ERROR_BUSY;
        return thisTarget->error;
    }
    thisTarget->numMaxRx = thisMaxRx;
    thisTarget->tmpNumRx = 0;
    thisTarget->rxBuffer = data;
    thisTarget->rxState = I2C_TARGET_STARTED;
    thisTarget->error = I2C_TARGET_ERROR_NONE;
    return thisTarget->error;
}

bool I2C_Target_TX_Done(I2C_Target *thisTarget) {
    return (thisTarget->txState == I2C_TARGET_IDLE);
}
bool I2C_Target_RX_Done(I2C_Target *thisTarget) {
    return (thisTarget->rxState == I2C_TARGET_IDLE);
}
void I2C_Target_IRQ(I2C_Target *thisTarget)
{
    I2C_Regs *thisPort = thisTarget->port;

    switch (DL_I2C_getPendingInterrupt(thisPort)) {
        case DL_I2C_IIDX_TARGET_START:
            if (thisTarget->rxState == I2C_TARGET_STARTED)
                thisTarget->rxState = I2C_TARGET_INPROGRESS;
            else
                thisTarget->rxState = I2C_TARGET_IDLE;
            if (thisTarget->txState == I2C_TARGET_STARTED)
                thisTarget->txState = I2C_TARGET_INPROGRESS;
            else
                thisTarget->txState = I2C_TARGET_IDLE;
            break;
        case DL_I2C_IIDX_TARGET_RXFIFO_TRIGGER:
            if (thisTarget->rxState == I2C_TARGET_INPROGRESS) {            
                while (DL_I2C_isTargetRXFIFOEmpty(thisPort) != true)
                {
                    if (thisTarget->tmpNumRx < thisTarget->numMaxRx)
                        thisTarget->rxBuffer[thisTarget->tmpNumRx++] = DL_I2C_receiveTargetData(thisPort);
                    else { // Controller sent more. Prevent buffer overflow
                        DL_I2C_receiveTargetData(thisPort);
                        thisTarget->error = I2C_TARGET_ERROR_RXBUFFER_LIMIT;
                    }
                }
            }
            else { // Ignore data if receive was not initialized
                thisTarget->error = I2C_TARGET_ERROR_IGNORE;
            }
            break;
        case DL_I2C_IIDX_TARGET_TXFIFO_TRIGGER:
            if (thisTarget->txState == I2C_TARGET_INPROGRESS) {
                if (thisTarget->tmpNumTx < thisTarget->numMaxTx)
                    thisTarget->tmpNumTx  += DL_I2C_fillTargetTXFIFO(thisPort, &(thisTarget->txBuffer[thisTarget->tmpNumTx]), (thisTarget->numMaxTx - thisTarget->tmpNumTx ));
                else { // Controller asked for more data 
                    while (DL_I2C_transmitTargetDataCheck(thisPort, 0x00) != false);
                    thisTarget->error = I2C_TARGET_ERROR_TXBUFFER_LIMIT;
                }
            }
           else { // Ignore data if transmit was not initialized
                thisTarget->error = I2C_TARGET_ERROR_IGNORE;
            }            
            break;
        case DL_I2C_IIDX_TARGET_STOP:
            if (thisTarget->rxState == I2C_TARGET_INPROGRESS) {
                thisTarget->rxState = I2C_TARGET_IDLE;
                thisTarget->rxCallBack(thisTarget->instanceNum);
            }
            if (thisTarget->txState == I2C_TARGET_INPROGRESS) {
                thisTarget->txState = I2C_TARGET_IDLE;
                thisTarget->txCallBack(thisTarget->instanceNum);
            }
            break;
        default:
            break;
    }
}
