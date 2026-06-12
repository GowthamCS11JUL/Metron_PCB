
#ifndef MODBUS_PROTOCOL_H_
#define MODBUS_PROTOCOL_H_

typedef enum
{
	WRITE_MULTIPLE_REGISTER = 0x10,
	READ_HOLDING_REGISTER=0x03,
	WRITE_SINGLE_REGISTER=0x06
} Modbus_Protocol_Function;


// modbus errors
typedef enum
{
	MB_NO_ERROR = 0,
	MB_ILLEGAL_FUNCTION=0x01,
	MB_ILLEGAL_DATA_ADDRESS,
	MB_ILLEGAL_DATA_VALUE,
	MB_SLAVE_DEVICE_FAILURE,
	MB_ACKNOWLEDGE,
	MB_SLAVE_DEVICE_BUSY,
	MB_MEMORY_PARITY_ERROR=0x08,
	MB_GATEWAY_PATH_UNAVAILABLE=0x0A,
	MB_TARGET_FAILED_TO_RESPOND=0x0B
} Modbus_RTU_Error;



// modbus pdu object
typedef struct
{
	Modbus_Protocol_Function function_code;
	uint8_t *data;
	uint8_t len;
} __attribute__((packed)) ModbusPdu;

// modbus adu object
typedef struct {
	uint8_t addr;
	ModbusPdu modbus_pdu;
	uint16_t crc;
} __attribute__((packed)) ModbusAdu;

typedef struct {
    // Address
	uint8_t modbusAddr; // modbus RTU address
	uint8_t groupAddr; // modbus RTU group address
	bool groupAddrEnable; // modbus RTU group address enable/disable

    // Data
	uint8_t *rx_data;//[RX_DATA_SIZE]; // rx data buffer
    uint16_t modbus_received_bytes; // number of received bytes during UART RX    
	uint8_t *tx_data;//[TX_DATA_SIZE]; // tx data buffer    
    uint16_t bytes_to_send;    
	ModbusAdu modbus_request; // modbus request object
	ModbusAdu modbus_response; // modbus response object

    // Registers
    uint16_t *illegalModbusAddresses;
    uint16_t numIllegalRegs;
    uint16_t *holdingRegisters;
    uint16_t numHoldingRegs;
    bool errorRegEnable;
    uint16_t errorRegAddr;

    // Call back function
    uint8_t (*modbusActionFunction)( uint16_t, uint8_t);
    bool txOk;

} Modbus_Protocol;

void modbusHandler(Modbus_Protocol *mb) ;
uint16_t modbusSilentInterval(int16_t baud_rate);// in us

#endif