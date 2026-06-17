#ifndef SPI_BLOCK_TRANSFER_H_
#define SPI_BLOCK_TRANSFER_H_


#include "ti/devices/msp/peripherals/hw_gpio.h"
#include <machine/_stdint.h>

typedef enum  {
    SPI_IDLE = 0,
    SPI_BUSY,
} SPI_Interface_State;

typedef enum {
    SPI_ERROR_NONE = 0,
    SPI_ERROR_RXBUFFER_FULL,
}SPI_Interface_Errror;

typedef struct {
    SPI_Regs *port;
    GPIO_Regs *gpio_port;
    uint8_t instanceNum;
    DMA_Regs *dma;
    uint8_t txDmaChannel;
    uint8_t rxDmaChannel;

    uint8_t *rxBuffer;
    uint8_t *txBuffer;
    uint16_t rxNum;
    uint16_t txNum;
    uint16_t tmpRxNum;
    uint16_t tmpTxNum;
    uint16_t csPin;
    bool rxDma;   
    bool txDma;   

    void (*txCallback)(uint8_t);
    void (*rxCallback)(uint8_t);  
    SPI_Interface_State rxState;
    SPI_Interface_State txState;
    SPI_Interface_Errror error;

} SPI_Block_Interface;

uint8_t SPI_Initialize_block_transfer(SPI_Block_Interface *thisInterface, SPI_Regs *thisPort,GPIO_Regs *this_gpio_port, uint8_t thisInstance, uint16_t this_csPin, uint8_t txDmaChannel, uint8_t rxDmaChannel);
void SPI_Transact_block(SPI_Block_Interface *thisInterface, uint8_t *txData, uint16_t numTx, uint8_t *rxData, uint16_t numRx);
void SPI_Block_IRQ(SPI_Block_Interface *thisInterface);
#endif /* SPI_BLOCK_TRANSFER_H_ */
