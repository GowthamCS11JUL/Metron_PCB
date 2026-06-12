/*
 * I2C_Controller.h
 *
 *  Created on: Apr 21, 2024
 *      Author: PramodVariyam
 */

#ifndef I2C_CONTROLLER_H_
#define I2C_CONTROLLER_H_



#include <sys/_stdint.h>
typedef enum  {
    I2C_CONTROLLER_IDLE = 0,
    I2C_CONTROLLER_BUSY,
} I2C_Controller_State;

typedef enum {
    I2C_CONTROLLER_ERROR_NONE = 0,
    I2C_CONTROLLER_ERROR_NACK,
    I2C_CONTROLLER_ERROR_DATA_ARBLOST,
}I2C_Controller_Errror;

typedef struct {
    I2C_Regs *port;
    uint8_t instanceNum;
     I2C_Controller_State state;
    I2C_Controller_Errror error;

    uint16_t numRx;
    uint16_t tmpNumRx;
    uint8_t *rxBuffer;
    void (*rxCallBack)(uint8_t);

    uint16_t numTx;
    uint16_t tmpNumTx;
    uint8_t *txBuffer;
    void (*txCallBack)(uint8_t);

} I2C_Controller;

I2C_Controller_Errror I2C_Controller_Initialize(I2C_Controller *thisController, I2C_Regs *thisPort, uint8_t thisInstance);
I2C_Controller_Errror I2C_Controller_Start_Transmit(I2C_Controller *thisController, uint8_t thisTargetAddress, uint8_t *thisTxBuffer, uint16_t thisCount);
I2C_Controller_Errror I2C_Controller_Start_Receive(I2C_Controller *thisController, uint8_t thisTarget, uint8_t *thisBuffer, uint16_t thisNumRxBytes);
void I2C_Controller_IRQ(I2C_Controller *thisController);
bool I2C_Controller_Transaction_Done(I2C_Controller *thisController) ;
#endif /* I2C_CONTROLLER_H_ */





