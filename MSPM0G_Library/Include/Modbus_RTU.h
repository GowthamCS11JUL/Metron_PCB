#ifndef MODBUS_RTU_H_
#define MODBUS_RTU_H_

typedef enum
{
	RTU_WRITE_MULTIPLE_REGISTER = 0x10,
	RTU_READ_HOLDING_REGISTER=0x03,
	RTU_WRITE_SINGLE_REGISTER=0x06
} Modbus_RTU_Function;



typedef struct {
    uint8_t rtuAddress;
    uint8_t rtuGroupAddress;
    bool rtuGroupAddressEnable;
    uint16_t numIllegalRegs;
    uint16_t numHoldingRegs;
    bool errorRegEnable;
    uint16_t errorRegAddress;
    uint16_t *illegalRegs;
    uint16_t *holdingRegs;
    uint8_t *rxData;
    uint8_t *txData;    
    uint8_t *requestAduData;
    uint8_t *responseAduData;
} Modbus_Protocol_Config;

// Modbus register data (Not including the holding register itself)
typedef struct {
	uint16_t address;
	uint8_t length;
	uint8_t (*callBack)(uint16_t, uint8_t);
} Modbus_RTU_Register;

typedef struct {
    Modbus_Protocol modbus;
    uint8_t uartTargetIndex;
    uint16_t numHoldingRegs;
    uint16_t *holdingRegs;
    
    uint16_t numRx;
    uint16_t maxNumRx;
    uint16_t numTx;

    Modbus_RTU_Register *holdingRegInfo;

} Modbus_RTU;

Modbus_RTU glModbusRtu;

uint32_t Modbus_RTU_Silent_Interval(uint32_t baudRate);
void Modbus_RTU_Initialize(Modbus_RTU *rtu, Modbus_Protocol_Config *config,  Modbus_RTU_Register *rtuRegs, uint8_t uartTargetIndex, uint16_t maxNumRx);
uint16_t Modbus_RTU_Get_Register(Modbus_RTU *rtu, uint16_t address, uint16_t *data ) ;
void Modbus_RTU_Add_Register(Modbus_RTU *rtu, uint16_t index, uint16_t address, uint16_t length, uint8_t (*callBack) (uint16_t, uint8_t));
void Modbus_RTU_FSM(Modbus_RTU *rtu);

#endif