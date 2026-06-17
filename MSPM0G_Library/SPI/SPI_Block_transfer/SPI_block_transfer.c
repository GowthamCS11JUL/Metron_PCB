#include <stdint.h>
#include <ti/driverlib/driverlib.h>
#include <SPI_Block_transfer.h>


void noActionSPIInterface(uint8_t num)
{
    __NOP();

}
void SPI_Block_rxDMA_Initialize(SPI_Block_Interface *thisInterface, uint8_t thisTrig)
{
    DL_DMA_setSrcAddr(thisInterface->dma, thisInterface->rxDmaChannel, (uint32_t)(&thisInterface->port->RXDATA));
    DL_DMA_configMode(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_SINGLE_TRANSFER_MODE, DL_DMA_NORMAL_MODE);
    DL_DMA_setDestIncrement (thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_ADDR_INCREMENT);
    DL_DMA_setDestWidth(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setSrcIncrement(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_ADDR_UNCHANGED);
    DL_DMA_setSrcWidth(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setTrigger(thisInterface->dma, thisInterface->rxDmaChannel, thisTrig, DL_DMA_TRIGGER_TYPE_EXTERNAL);
}
void SPI_Block_txDMA_Initialize(SPI_Block_Interface *thisInterface, uint8_t thisTrig) {
    DL_DMA_setDestAddr(thisInterface->dma, thisInterface->txDmaChannel, (uint32_t)(&thisInterface->port->TXDATA));
    DL_DMA_configMode(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_SINGLE_TRANSFER_MODE, DL_DMA_NORMAL_MODE);
    DL_DMA_setDestIncrement (thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_ADDR_UNCHANGED);
    DL_DMA_setDestWidth(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setSrcIncrement(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_ADDR_INCREMENT);
    DL_DMA_setSrcWidth(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setTrigger(thisInterface->dma, thisInterface->txDmaChannel, thisTrig, DL_DMA_TRIGGER_TYPE_EXTERNAL);
}

uint8_t SPI_Initialize_block_transfer(SPI_Block_Interface *thisInterface, SPI_Regs *thisPort , GPIO_Regs *this_gpio_port, uint8_t thisInstance, uint16_t this_csPin, uint8_t txDmaChannel, uint8_t rxDmaChannel)
{
    uint8_t txTrig, rxTrig;
    thisInterface->port = thisPort;
    thisInterface->instanceNum = thisInstance;
    thisInterface->gpio_port = this_gpio_port;
    thisInterface->dma = DMA;
    thisInterface->csPin = this_csPin;
    thisInterface->rxCallback = noActionSPIInterface;
    thisInterface->txCallback = noActionSPIInterface;    
    thisInterface->rxState = SPI_IDLE;
    thisInterface->txState = SPI_IDLE;
    thisInterface->error = SPI_ERROR_NONE;
    thisInterface->rxNum = 0;
    thisInterface->txNum = 0;
    thisInterface->tmpRxNum = 0;
    thisInterface->tmpTxNum = 0;
    thisInterface->rxDmaChannel = rxDmaChannel;    
    thisInterface->rxDma = (rxDmaChannel < 16) ;
    thisInterface->txDmaChannel = txDmaChannel;    
    thisInterface->txDma = (txDmaChannel < 16) ;
    if (thisInstance == 0)
    {
        NVIC_ClearPendingIRQ(SPI0_INT_IRQn);
        NVIC_EnableIRQ(SPI0_INT_IRQn);
        txTrig = DMA_SPI0_TX_TRIG;
        rxTrig = DMA_SPI0_RX_TRIG;
    }
    if (thisInstance == 1)
    {
        NVIC_ClearPendingIRQ(SPI1_INT_IRQn);
        NVIC_EnableIRQ(SPI1_INT_IRQn);
        txTrig = DMA_SPI1_TX_TRIG;
        rxTrig = DMA_SPI1_RX_TRIG;
    }
    if (thisInterface->rxDma)
        SPI_Block_rxDMA_Initialize(thisInterface, rxTrig);
    if (thisInterface->txDma)
        SPI_Block_txDMA_Initialize(thisInterface, txTrig);  

    return thisInterface->error;
}

void SPI_Controller_Set_CS_Low(SPI_Block_Interface *thisInterface) {
    DL_GPIO_clearPins(thisInterface->gpio_port, thisInterface->csPin);
}
 
void SPI_Controller_Set_CS_High(SPI_Block_Interface *thisInterface) {
    DL_GPIO_setPins(thisInterface->gpio_port, thisInterface->csPin);
}

void SPI_Transact_block(SPI_Block_Interface *thisInterface, uint8_t *txData, uint16_t numTx, uint8_t *rxData, uint16_t numRx)
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
    if (thisInterface->rxDma) {
        DL_DMA_setDestAddr(thisInterface->dma, thisInterface->rxDmaChannel, (uint32_t)(&thisInterface->rxBuffer[0]));
        DL_DMA_setTransferSize(thisInterface->dma, thisInterface->rxDmaChannel, thisInterface->rxNum);
        DL_DMA_enableChannel(thisInterface->dma, thisInterface->rxDmaChannel);
    }
    else {
        DL_SPI_drainRXFIFO16(thisInterface->port, dummy,4);
    }

    //Set CS pin low for transmission 
    SPI_Controller_Set_CS_Low(thisInterface);

    if (thisInterface->txDma) {
        DL_DMA_setSrcAddr(thisInterface->dma, thisInterface->txDmaChannel,(uint32_t)(&thisInterface->txBuffer[0]));
        DL_DMA_setTransferSize(thisInterface->dma, thisInterface->txDmaChannel, thisInterface->txNum);
        DL_DMA_enableChannel(thisInterface->dma, thisInterface->txDmaChannel);
    }
    else {
        DL_SPI_transmitData16(thisInterface->port, thisInterface->txBuffer[thisInterface->tmpTxNum++]);
    }
}

void SPI_Block_IRQ(SPI_Block_Interface *thisInterface)
{
    switch (DL_SPI_getPendingInterrupt(thisInterface->port)) {
        case DL_SPI_IIDX_DMA_DONE_TX:
            if (thisInterface->txDma) {
                thisInterface->txState = SPI_IDLE;
                thisInterface->txCallback(thisInterface->instanceNum);
            }
        case DL_SPI_IIDX_DMA_DONE_RX:
            if (thisInterface->rxDma) {
                thisInterface->rxState = SPI_IDLE;
                thisInterface->rxCallback(thisInterface->instanceNum);
                //Set CS pin high to end of transmission
                SPI_Controller_Set_CS_High(thisInterface);
            }    
        case DL_SPI_IIDX_IDLE:
            // Receive data from buffer first
            if (!thisInterface->rxDma) {
            if (thisInterface->tmpRxNum < thisInterface->rxNum)
               thisInterface->rxBuffer[thisInterface->tmpRxNum++] = DL_SPI_receiveData16(thisInterface->port);
            else if (thisInterface->tmpRxNum == thisInterface->rxNum) {
                thisInterface->rxState = SPI_IDLE; 
                thisInterface->rxCallback(thisInterface->instanceNum);
            }
            }
            // Now transmit data
            if (!thisInterface->txDma) {
            if (thisInterface->tmpTxNum < thisInterface->txNum)
                DL_SPI_transmitData16(thisInterface->port, thisInterface->txBuffer[thisInterface->tmpTxNum++]);
            else if (thisInterface->tmpTxNum == thisInterface->txNum) {
                thisInterface->txState = SPI_IDLE; 
                
                //Set CS pin high to end of transmission
                SPI_Controller_Set_CS_High(thisInterface);

                thisInterface->txCallback(thisInterface->instanceNum);
            }
            }
            break;   
        case DL_SPI_IIDX_RX:  
        case DL_SPI_IIDX_TX:
        default:
            break;
    }
}