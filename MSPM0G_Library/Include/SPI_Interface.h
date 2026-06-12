/*
 * SPI_Interface.h
 *
 *  Created on: Apr 26, 2024
 *      Author: PramodVariyam
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_


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
    uint8_t instanceNum;

    uint16_t *rxBuffer;
    uint16_t *txBuffer;
    uint16_t rxNum;
    uint16_t txNum;
    uint16_t tmpRxNum;
    uint16_t tmpTxNum;

    void (*txCallback)(uint8_t);
    void (*rxCallback)(uint8_t);  
    SPI_Interface_State rxState;
    SPI_Interface_State txState;
    SPI_Interface_Errror error;

} SPI_Interface;

uint8_t SPI_Initialize(SPI_Interface *thisInterface, SPI_Regs *thisPort, uint8_t thisInstance);
void SPI_Transact(SPI_Interface *thisInterface, uint16_t *txData, uint16_t numTx, uint16_t *rxData, uint16_t numRx);
void SPI_IRQ(SPI_Interface *thisInterface);
void SPI_Controller_Set_CS(SPI_Interface *thisInterface, uint8_t csNum);
#endif /* SPI_SPI_H_ */
