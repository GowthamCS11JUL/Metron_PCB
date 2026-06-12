
#include <ti/driverlib/driverlib.h>
#include <SPI_Interface.h>


void noActionSPIInterface(uint8_t num)
{
    __NOP();

}


uint8_t SPI_Initialize(SPI_Interface *thisInterface, SPI_Regs *thisPort, uint8_t thisInstance)
{
    thisInterface->port = thisPort;
    thisInterface->instanceNum = thisInstance;
    thisInterface->rxCallback = noActionSPIInterface;
    thisInterface->txCallback = noActionSPIInterface;    
    thisInterface->rxState = SPI_IDLE;
    thisInterface->txState = SPI_IDLE;
    thisInterface->error = SPI_ERROR_NONE;
    thisInterface->rxNum = 0;
    thisInterface->txNum = 0;
    thisInterface->tmpRxNum = 0;
    thisInterface->tmpTxNum = 0;
    if (thisInstance == 0)
    {
        NVIC_ClearPendingIRQ(SPI0_INT_IRQn);
        NVIC_EnableIRQ(SPI0_INT_IRQn);
    }
    if (thisInstance == 1)
    {
        NVIC_ClearPendingIRQ(SPI1_INT_IRQn);
        NVIC_EnableIRQ(SPI1_INT_IRQn);
    }

    return thisInterface->error;
}

void SPI_Controller_Set_CS(SPI_Interface *thisInterface, uint8_t csNum)
{

    DL_SPI_setChipSelect(thisInterface->port, DL_SPI_CHIP_SELECT_0 + ( DL_SPI_CHIP_SELECT )csNum);
}

void SPI_Transact(SPI_Interface *thisInterface, uint16_t *txData, uint16_t numTx, uint16_t *rxData, uint16_t numRx)
{
    uint16_t dummy[4];
    thisInterface->error = SPI_ERROR_NONE;
    thisInterface->txBuffer = txData;
    thisInterface->rxBuffer = rxData;
    SPI_Regs *thisPort = thisInterface->port;
    thisInterface->rxState = SPI_BUSY;
    thisInterface->txState = SPI_BUSY;
    thisInterface->rxNum = numRx;
    thisInterface->txNum = numTx;
    thisInterface->tmpRxNum = 0;
    thisInterface->tmpTxNum = 0;
    DL_SPI_drainRXFIFO16(thisInterface->port, dummy,4);
    DL_SPI_transmitData16(thisInterface->port, thisInterface->txBuffer[thisInterface->tmpTxNum++]);
}


void SPI_IRQ(SPI_Interface *thisInterface)
{
    switch (DL_SPI_getPendingInterrupt(thisInterface->port)) {
        case DL_SPI_IIDX_IDLE:
            // Receive data from buffer first
            if (thisInterface->tmpRxNum < thisInterface->rxNum)
               thisInterface->rxBuffer[thisInterface->tmpRxNum++] = DL_SPI_receiveData16(thisInterface->port);
            else if (thisInterface->tmpRxNum == thisInterface->rxNum) {
                thisInterface->rxState = SPI_IDLE; 
                thisInterface->rxCallback(thisInterface->instanceNum);
            }
            // Now transmit data
            if (thisInterface->tmpTxNum < thisInterface->txNum)
                DL_SPI_transmitData16(thisInterface->port, thisInterface->txBuffer[thisInterface->tmpTxNum++]);
            else if (thisInterface->tmpTxNum == thisInterface->txNum) {
                thisInterface->txState = SPI_IDLE; 
                thisInterface->txCallback(thisInterface->instanceNum);
            }
            break;   
        case DL_SPI_IIDX_RX:  
        case DL_SPI_IIDX_TX:
        default:
            break;
    }
}
