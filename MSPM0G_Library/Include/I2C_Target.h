

#ifndef I2C_TARGET_H_
#define I2C_TARGET_H_

#include <sys/_stdint.h>
typedef enum  {
        I2C_TARGET_IDLE = 0,
        I2C_TARGET_STARTED,
        I2C_TARGET_INPROGRESS,
    } I2C_Target_State;

    typedef enum {
        I2C_TARGET_ERROR_NONE = 0,
        I2C_TARGET_ERROR_BUSY,
        I2C_TARGET_ERROR_IGNORE,
        I2C_TARGET_ERROR_RXBUFFER_LIMIT,
        I2C_TARGET_ERROR_TXBUFFER_LIMIT
    }I2C_Target_Errror;

    typedef struct {
        I2C_Regs *port;
        uint8_t instanceNum;
        I2C_Target_Errror error;
        uint8_t address;
        uint16_t numMaxRx;
        uint16_t tmpNumRx;
        uint8_t *rxBuffer;
        I2C_Target_State rxState;
        void (*rxCallBack)(uint8_t);

        uint16_t numMaxTx;
        uint16_t tmpNumTx;
        uint8_t *txBuffer;
        I2C_Target_State txState;
        void (*txCallBack)(uint8_t);

    } I2C_Target;

void I2C_Target_Initialize(I2C_Target *thisTarget, I2C_Regs *thisPort, uint8_t thisInstance, uint8_t thisAddress);
I2C_Target_Errror I2C_Target_Setup_Transmit(I2C_Target *thisTarget, uint8_t *data, uint16_t numMaxTx);
I2C_Target_Errror I2C_Target_Setup_Recieve(I2C_Target *thisTarget, uint8_t *data, uint16_t thisMaxRx);
bool I2C_Target_TX_Done(I2C_Target *thisTarget);
bool I2C_Target_RX_Done(I2C_Target *thisTarget);
void I2C_Target_IRQ(I2C_Target *thisTarget);


#endif /* I2C_TARGET_H_ */
