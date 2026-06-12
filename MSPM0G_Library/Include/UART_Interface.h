
#ifndef UART_INTERFACE_H_
#define UART_INTERFACE_H_




typedef enum  {
    UART_IDLE = 0,
    UART_BUSY,
} UART_Interface_State;

typedef enum {
    UART_ERROR_NONE = 0,
    UART_ERROR_WRONG_INSTANCE,
    UART_ERROR_STILL_BUSY,
    UART_ERROR_PROTOCOL,
}UART_Interface_Error;

typedef struct {
    UART_Regs *port;
    DMA_Regs *dma;
    uint8_t instanceNum;
    uint8_t txDmaChannel;
    uint8_t rxDmaChannel;

    UART_Interface_Error error;

    uint8_t *rxBuffer;
    uint16_t numRx;
    uint16_t tmpNumRx;
    UART_Interface_State rxState;
    bool rxDma;    
    bool rxTimeout;    
    void (*rxInProgressCallback)(uint8_t, uint16_t);
    void (*rxDoneCallback)(uint8_t, uint16_t);


    uint8_t *txBuffer;
    uint16_t numTx;
    uint16_t tmpNumTx;
    UART_Interface_State txState;   
    bool txTimeout;    
    bool txDma;   
    void (*txInProgressCallback)(uint8_t,uint16_t);
    void (*txDoneCallback)(uint8_t, uint16_t);

} UART_Interface;

UART_Interface_Error UART_Interface_Initialize(UART_Interface *thisInterface, UART_Regs *thisPort, uint8_t thisInstance, uint8_t txDmaChannel, uint8_t rxDmaChannel);
UART_Interface_Error UART_Interface_Start_Transmit(UART_Interface *thisInterface, uint8_t *data, uint16_t numSamples);
UART_Interface_Error UART_Interface_Start_Receive(UART_Interface *thisInterface, uint8_t *data, uint16_t numSamples);
UART_Interface_Error UART_Interface_Stop_Receive(UART_Interface *thisInterface);
bool UART_Interface_RX_Done(UART_Interface *thisInterface);
bool UART_Interface_TX_Done(UART_Interface *thisInterface);
void UART_Interface_IRQ(UART_Interface *thisInterface);
uint16_t UART_Interface_NumRX(UART_Interface *thisInterface);
uint16_t UART_Interface_NumTX(UART_Interface *thisInterface);

#endif
