#include "ti/devices/msp/peripherals/hw_dma.h"
#include <ti/driverlib/m0p/dl_core.h>
#include <ti/driverlib/driverlib.h>
#include <UART_Interface.h>



void defaultUartCallBack(uint8_t dummy,uint16_t tmp)
{
    __NOP();
}

void defaultUartInprogressCallback(uint8_t dummy,uint16_t tmp) {
    __NOP();
}
void UART_rxDMA_Initialize(UART_Interface *thisInterface, uint8_t thisTrig)
{
    DL_DMA_setSrcAddr(thisInterface->dma, thisInterface->rxDmaChannel, (uint32_t)(&thisInterface->port->RXDATA));
    DL_DMA_configMode(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_SINGLE_TRANSFER_MODE, DL_DMA_NORMAL_MODE);
    DL_DMA_setDestIncrement (thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_ADDR_INCREMENT);
    DL_DMA_setDestWidth(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setSrcIncrement(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_ADDR_UNCHANGED);
    DL_DMA_setSrcWidth(thisInterface->dma, thisInterface->rxDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setTrigger(thisInterface->dma, thisInterface->rxDmaChannel, thisTrig, DL_DMA_TRIGGER_TYPE_EXTERNAL);
}
void UART_txDMA_Initialize(UART_Interface *thisInterface, uint8_t thisTrig) {
    DL_DMA_setDestAddr(thisInterface->dma, thisInterface->txDmaChannel, (uint32_t)(&thisInterface->port->TXDATA));
    DL_DMA_configMode(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_SINGLE_TRANSFER_MODE, DL_DMA_NORMAL_MODE);
    DL_DMA_setDestIncrement (thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_ADDR_UNCHANGED);
    DL_DMA_setDestWidth(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setSrcIncrement(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_ADDR_INCREMENT);
    DL_DMA_setSrcWidth(thisInterface->dma, thisInterface->txDmaChannel,DL_DMA_WIDTH_BYTE);
    DL_DMA_setTrigger(thisInterface->dma, thisInterface->txDmaChannel, thisTrig, DL_DMA_TRIGGER_TYPE_EXTERNAL);
}

UART_Interface_Error UART_Interface_Initialize(UART_Interface *thisInterface, UART_Regs *thisPort, uint8_t thisInstance, uint8_t txDmaChannel, uint8_t rxDmaChannel)
{
    uint8_t txTrig, rxTrig;
    thisInterface->port = thisPort;
    thisInterface->dma = DMA;
    thisInterface->instanceNum = thisInstance;
    thisInterface->error = UART_ERROR_NONE;

    thisInterface->numRx = 0;
    thisInterface->tmpNumRx = 0;    
    thisInterface->rxState = UART_IDLE;
    thisInterface->rxTimeout = false;
    thisInterface->rxDoneCallback = defaultUartCallBack;
    thisInterface->rxInProgressCallback = defaultUartInprogressCallback;
    thisInterface->rxDmaChannel = rxDmaChannel;    
    thisInterface->rxDma = (rxDmaChannel < 16) ;

    thisInterface->numTx = 0;
    thisInterface->numTx = 0;   
    thisInterface->txState = UART_IDLE;
    thisInterface->txTimeout = false;    
    thisInterface->txDoneCallback = defaultUartCallBack;
    thisInterface->txInProgressCallback = defaultUartInprogressCallback;
    thisInterface->txDmaChannel = txDmaChannel;
    thisInterface->txDma = (txDmaChannel < 16)  ;      
     
    if (thisInstance == 0){
        NVIC_ClearPendingIRQ(UART0_INT_IRQn);
        NVIC_EnableIRQ(UART0_INT_IRQn);
        txTrig = DMA_UART0_TX_TRIG;
        rxTrig = DMA_UART0_RX_TRIG;
    }
    else if (thisInstance == 1){
        NVIC_ClearPendingIRQ(UART1_INT_IRQn);
        NVIC_EnableIRQ(UART1_INT_IRQn);
        txTrig = DMA_UART1_TX_TRIG;
        rxTrig = DMA_UART1_RX_TRIG;        
    }
    else if (thisInstance == 2){
        NVIC_ClearPendingIRQ(UART2_INT_IRQn);
        NVIC_EnableIRQ(UART2_INT_IRQn);
        txTrig = DMA_UART2_TX_TRIG;
        rxTrig = DMA_UART2_RX_TRIG;        
    }
else if (thisInstance == 3)
{
    NVIC_ClearPendingIRQ(UART3_INT_IRQn);
    NVIC_EnableIRQ(UART3_INT_IRQn);

    txTrig = DMA_UART3_TX_TRIG;
    rxTrig = DMA_UART3_RX_TRIG;
}
    else {
        thisInterface->error = UART_ERROR_WRONG_INSTANCE; 
    }
    if (thisInterface->rxDma)
        UART_rxDMA_Initialize(thisInterface, rxTrig);
    if (thisInterface->txDma)
        UART_txDMA_Initialize(thisInterface, txTrig);    

    return thisInterface->error;
}


UART_Interface_Error UART_Interface_Start_Transmit(UART_Interface *thisInterface, uint8_t *data, uint16_t numSamples)
{
    if (thisInterface->txState != UART_IDLE) {
        thisInterface->error = UART_ERROR_STILL_BUSY;
        return thisInterface->error;
    }
    thisInterface->error = UART_ERROR_NONE;
    thisInterface->numTx = numSamples;
    thisInterface->tmpNumTx = 0;
    thisInterface->txState = UART_BUSY;
    thisInterface->txBuffer = data;
    thisInterface->txTimeout = false;
    if (thisInterface->txDma) {
        DL_DMA_setSrcAddr(thisInterface->dma, thisInterface->txDmaChannel, (uint32_t) (&thisInterface->txBuffer[0]));
        DL_DMA_setTransferSize(thisInterface->dma, thisInterface->txDmaChannel, numSamples);
        DL_DMA_enableChannel(thisInterface->dma, thisInterface->txDmaChannel);
    }
    else {
        thisInterface->tmpNumTx = DL_UART_fillTXFIFO(thisInterface->port, thisInterface->txBuffer, (uint32_t) thisInterface->numTx);
    }
    return thisInterface->error;
}

UART_Interface_Error UART_Interface_Start_Receive(UART_Interface *thisInterface, uint8_t *data, uint16_t numSamples)
{
    uint8_t dummy[4];
    if (thisInterface->rxState != UART_IDLE) {
        thisInterface->error = UART_ERROR_STILL_BUSY;
        return thisInterface->error;
    }
    // Flush existing buffer
    while (DL_UART_drainRXFIFO(thisInterface->port, &dummy[0],4) > 0);
    thisInterface->error = UART_ERROR_NONE;
    thisInterface->numRx = numSamples;
    thisInterface->tmpNumRx = 0;
    thisInterface->rxState = UART_BUSY;
    thisInterface->rxBuffer = data;
    thisInterface->rxTimeout = false;
    if (thisInterface->rxDma) {
        DL_DMA_setDestAddr(thisInterface->dma, thisInterface->rxDmaChannel, (uint32_t) (&thisInterface->rxBuffer[0]));
        DL_DMA_setTransferSize(thisInterface->dma, thisInterface->rxDmaChannel, numSamples);
        DL_DMA_enableChannel(thisInterface->dma, thisInterface->rxDmaChannel);
    }
    return thisInterface->error;
}

void UART_Interface_IRQ(UART_Interface *thisInterface)
{
    switch (DL_UART_getPendingInterrupt(thisInterface->port))
    {
         case DL_UART_MAIN_IIDX_EOT_DONE:
            break;
        case DL_UART_MAIN_IIDX_DMA_DONE_TX:
            if (thisInterface->txDma) {
                thisInterface->txState = UART_IDLE;
                thisInterface->txDoneCallback(thisInterface->instanceNum, thisInterface->numTx); 
            }
            break;
        case DL_UART_MAIN_IIDX_DMA_DONE_RX:
            if (thisInterface->rxDma) {
                thisInterface->rxState = UART_IDLE;
                thisInterface->rxDoneCallback(thisInterface->instanceNum, thisInterface->numRx); 
            }
            break;            
        case DL_UART_IIDX_RX :
            if ((thisInterface->numRx == 0) || (thisInterface->rxState == UART_IDLE))
                break;        
            if (!thisInterface->rxDma) {
                if (thisInterface->tmpNumRx < thisInterface->numRx) {
                    thisInterface->tmpNumRx   += DL_UART_drainRXFIFO(thisInterface->port, &(thisInterface->rxBuffer[thisInterface->tmpNumRx]),(uint32_t) (thisInterface->numRx - thisInterface->tmpNumRx));
                     thisInterface->rxInProgressCallback(thisInterface->instanceNum, thisInterface->tmpNumRx);
                }
                if (thisInterface->tmpNumRx == thisInterface->numRx) {
                    thisInterface->rxState = UART_IDLE;
                    thisInterface->rxDoneCallback(thisInterface->instanceNum, thisInterface->numRx);
                }
            }
            else {
                thisInterface->tmpNumRx++;
                thisInterface->rxInProgressCallback(thisInterface->instanceNum, thisInterface->tmpNumRx);
            }
            
            break;
        case DL_UART_IIDX_TX:
            if ((thisInterface->numTx == 0) || (thisInterface->txState == UART_IDLE))
                break;
            if (!thisInterface->txDma) {
                if (thisInterface->tmpNumTx < thisInterface->numTx) {
                    thisInterface->tmpNumTx += DL_UART_fillTXFIFO(thisInterface->port, &(thisInterface->txBuffer[thisInterface->tmpNumTx]), (uint32_t) (thisInterface->numTx - thisInterface->tmpNumTx));
                    thisInterface->txInProgressCallback(thisInterface->instanceNum, thisInterface->tmpNumTx);
                }
                if (thisInterface->tmpNumTx == thisInterface->numTx) {
                    thisInterface->txState = UART_IDLE;      
                    thisInterface->txDoneCallback(thisInterface->instanceNum, thisInterface->numTx); 
                }
            }
            else {
                thisInterface->tmpNumTx++;
                thisInterface->txInProgressCallback(thisInterface->instanceNum, thisInterface->tmpNumTx);
            }
            break;
        case DL_UART_IIDX_NOISE_ERROR:
        case DL_UART_IIDX_RX_TIMEOUT_ERROR:
        case DL_UART_IIDX_FRAMING_ERROR:
        case DL_UART_IIDX_PARITY_ERROR:
        case DL_UART_IIDX_BREAK_ERROR:
        case DL_UART_IIDX_OVERRUN_ERROR:
            thisInterface->error = UART_ERROR_PROTOCOL;
        default:
            break;
    }
}

UART_Interface_Error UART_Interface_Stop_Receive(UART_Interface *thisInterface) {
    thisInterface->rxState = UART_IDLE;
    if (thisInterface->rxDma) {
        DL_DMA_disableChannel(thisInterface->dma, thisInterface->rxDmaChannel);
    }
    thisInterface->rxDoneCallback(thisInterface->instanceNum, thisInterface->tmpNumRx); 
    return thisInterface->error;
}

uint16_t UART_Interface_NumRX(UART_Interface *thisInterface) {
    return thisInterface->tmpNumRx;
}

uint16_t UART_Interface_NumTX(UART_Interface *thisInterface) {
    return thisInterface->tmpNumTx;
}


bool UART_Interface_RX_Done(UART_Interface *thisInterface) {
    return (thisInterface->rxState == UART_IDLE);
}

bool UART_Interface_TX_Done(UART_Interface *thisInterface) {
    return (thisInterface->txState == UART_IDLE);
}