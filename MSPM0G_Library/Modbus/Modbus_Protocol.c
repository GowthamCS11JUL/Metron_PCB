#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "Modbus_Protocol.h"


/*
	modbusSilentInterval
	Returns a CMP value for a timer for modbus communication
	
	cpu_clk - Clock speed (in hz) of the main CPU
	timer_div - Time division used for the clock speed
	baud_rate - Baud rate of the UART module
*/

uint16_t modbusSilentInterval(int16_t baud_rate)
{
	uint16_t cmp;
	cmp = (uint16_t) (35*1000000.0/baud_rate) + 1; // 3.5 chracters => 3.5*10
	return cmp;
}


/*
    crc16_update
    Returns the CRC operation on one byte input 

*/

uint16_t crc16_update(uint16_t crc, uint8_t a) {
    int i;
    crc ^= (uint16_t)a;
    for (i = 0; i < 8; ++i) {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    return crc;
}
/*
	modbusCrcCalc
	returns the calculated crc of a given array
	
	data - pointer to uint8_t array
	len - length of array
*/

uint16_t modbusCrcCalc(uint8_t* data, uint8_t len) {
	uint8_t i = 0;
	uint16_t crc = 0xFFFF;

	for(i=0; i < len; i++)
		crc = crc16_update(crc, data[i]);
	return crc;
} 

/*
	modbusCrcCheck
	Check a recieved modbus packet's CRC with a calculated CRC
	
	data - received modbus packet (with crc included)
	len - length of received modbus packet
*/
uint8_t modbusCrcCheck(uint8_t* data, uint8_t len) {
    uint16_t calculated_crc = modbusCrcCalc( data, len-2);
	uint16_t received_crc = (((uint16_t)data[len-1]) << 8) | data[len - 2];
	return (calculated_crc == received_crc) ? 1 : 0;
}

/*
	modbusRequestAdu
	Put together the modbus request from the recieved raw packet
	
	mb - Pointer to the main modbus object
*/
uint8_t modbusRequestAdu(Modbus_Protocol *mb) {

	if(mb->groupAddrEnable) { // if group enabled and addresses does not match, return
		if ((mb->rx_data[0] != mb->modbusAddr) && (mb->rx_data[0] != 0) && (mb->rx_data[0] != mb->groupAddr)) {
			return 0;
		}
	}
	else { // if group not enabled and address does not match return
		if ((mb->rx_data[0] != mb->modbusAddr) && (mb->rx_data[0] != 0)) {
			return 0;
		}
	}
	if(modbusCrcCheck(&mb->rx_data[0], mb->modbus_received_bytes) != 1) // check crc
	    return 0;
	
	mb->modbus_request.addr = mb->rx_data[0]; // send address to object
	mb->modbus_request.modbus_pdu.function_code = mb->rx_data[1]; // send function code to object
	mb->modbus_request.modbus_pdu.len = mb->modbus_received_bytes - 4; // send length to object
	
	//memcpy(&mb->modbus_request.modbus_pdu.data, &mb->rx_data[2], mb->modbus_request.modbus_pdu.len); // copy data from buffer to object
    mb->modbus_request.modbus_pdu.data=(uint8_t*) &mb->rx_data[2];
	memcpy(&mb->modbus_request.crc, &mb->rx_data[(mb->modbus_request.modbus_pdu.len + 1)], 2); // copy data from crc to object
	
	return 1;
}

/*
	modbusResponseAdu
	Take the response data and put it into the transmit buffer
	
	mb - Pointer to the main modbus object
*/
uint8_t modbusResponseAdu(Modbus_Protocol* mb) {
	uint16_t i = 0;
//	for (i=0; i<TX_DATA_SIZE; i++) { // reset tx_data array 
//		mb->tx_data[i] = 0x00;
//	}
	mb->tx_data[0] = mb->modbus_response.addr; // put response addr in tx buffer
	mb->tx_data[1] = mb->modbus_response.modbus_pdu.function_code; // put response function code in tx buffer
	memcpy(&mb->tx_data[2], &mb->modbus_response.modbus_pdu.data[0], mb->modbus_response.modbus_pdu.len); // copy data to tx buffer
	uint8_t len = mb->modbus_response.modbus_pdu.len + 2; // get response len
	uint16_t crc = modbusCrcCalc(&mb->tx_data[0], len); // get response crc
	mb->tx_data[len] = crc & 0xFF; // add on crc high to buffer
	mb->tx_data[len+1] = (crc >> 8) & 0xFF; // add on crc low to buffer
	
	return mb->modbus_response.modbus_pdu.len + 4; // return length
}

/*
	illegalModbusAddress
	Loop through all illegal addresses for the modbus object and return true if the passed address
	is illegal and false if the passed address is legal.
	
	mb - Pointer to the main modbus object
	modbus_addr - address to check for validity
*/
bool illegalModbusAddress(Modbus_Protocol *mb, uint16_t modbus_addr) {
	if(modbus_addr == 0) return false; // address 0 can never be illegal
	for(int i=0; i<mb->numIllegalRegs; i++) {
		if (modbus_addr == mb->illegalModbusAddresses[i]) {
			return true;
		}
	}
	return false;
}


/*
	readHoldingRegisters
	Functionality for a READ_HOLDING_REGISTER function code
	
	mb - Pointer to the main modbus object
*/
Modbus_RTU_Error readHoldingRegisters(Modbus_Protocol *mb)
{
	ModbusPdu *request = &mb->modbus_request.modbus_pdu;
	ModbusPdu *response = &mb->modbus_response.modbus_pdu;
	uint16_t start_address, register_len;
	uint8_t byte_count;
	uint8_t return_code = MB_NO_ERROR; // initialize error
	uint16_t index, data_index, end_address;
	
	register_len = ((request->data[2] << 8) | request->data[3]); // get length of reg
	if ((register_len < 1) | (register_len > 0x007D)) // make sure length isn't 0 or too large
	{
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_ILLEGAL_DATA_VALUE;
		response->len = 1;
		return MB_ILLEGAL_DATA_VALUE;
	}
	
	start_address = ((request->data[0] << 8) | request->data[1]); // get start addr
	end_address = start_address + register_len; // calculate end addr
	
	// if end address is greater than max or we're dealing with an illegal address
	if (end_address > mb->numHoldingRegs || illegalModbusAddress(mb, start_address))
	{
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_ILLEGAL_DATA_ADDRESS;
		response->len = 1;
		return MB_ILLEGAL_DATA_ADDRESS;
	}
	
	// execute action function
	return_code=  mb->modbusActionFunction(start_address, READ_HOLDING_REGISTER);
	if (return_code != MB_NO_ERROR)
	{
		if(mb->errorRegEnable) {
			mb->holdingRegisters[mb->errorRegAddr] = return_code;
		}
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_SLAVE_DEVICE_FAILURE;
		response->len = 1;
		return MB_SLAVE_DEVICE_FAILURE;
	}
	// function code
	response->function_code = 	READ_HOLDING_REGISTER;
	// number of bytes = register length * 2
	byte_count = register_len << 1;
	response->data[0]= byte_count;
	
	// copy data over from holding registers
	data_index = 1;
	for (index=start_address; index<end_address; index++)
	{
		response->data[data_index] = (mb->holdingRegisters[index] >> 8) & (0xFF);
		response->data[data_index+1] = (mb->holdingRegisters[index]) & (0xFF);
		data_index = data_index + 2;
	}
	

	// length
	response->len = byte_count + 1 ;//register_len + 1;
	
	return return_code;
}

/*
	writeSingleRegister
	Functionality for a WRITE_SINGLE_REGISTER function code
	
	mb - Pointer to the main modbus object
*/
Modbus_RTU_Error writeSingleRegister(Modbus_Protocol *mb)
{
	ModbusPdu *request = &mb->modbus_request.modbus_pdu;
	ModbusPdu *response = &mb->modbus_response.modbus_pdu;
	uint16_t register_address, register_data;
	uint8_t return_code = MB_NO_ERROR; // initialize error

	register_data = ((request->data[2] << 8) | request->data[3]); // get data
	register_address = ((request->data[0] << 8) | request->data[1]); // get register address
	
	// if reg address is illegal
	if (register_address > mb->numHoldingRegs || illegalModbusAddress(mb, register_address))
	{
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_ILLEGAL_DATA_ADDRESS;
		response->len = 1;
		return MB_ILLEGAL_DATA_ADDRESS;
		
	}
	// Copy the data to holding registers
	mb->holdingRegisters[register_address]=register_data;
	// Run the action for the address
	
	return_code=  mb->modbusActionFunction(register_address, WRITE_SINGLE_REGISTER);
	
	// If the action returns an error, send response
	if (return_code != MB_NO_ERROR)
	{
		if(mb->errorRegEnable) {
			mb->holdingRegisters[mb->errorRegAddr] = return_code;
		}
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_SLAVE_DEVICE_FAILURE;
		response->len = 1;
		return MB_SLAVE_DEVICE_FAILURE;
	}
	
	// function code
	response->function_code = 	request->function_code;
	response->data[0] = request->data[0];
	response->data[1] = request->data[1];
	response->data[2] = request->data[2] ;
	response->data[3] = request->data[3];
	response->len = 4;

	return return_code;
}

/*
	writeMultipleRegister
	Functionality for a WRITE_MULTIPLE_REGISTER function code
	
	mb - Pointer to the main modbus object
*/
Modbus_RTU_Error writeMultipleRegister(Modbus_Protocol *mb)
{
	ModbusPdu *request = &mb->modbus_request.modbus_pdu;
	ModbusPdu *response = &mb->modbus_response.modbus_pdu;
	uint16_t start_address, register_quantity, num_bytes, end_address;
	uint8_t return_code = MB_NO_ERROR; // initialize error
	uint16_t index, data_index;
	
	register_quantity = ((request->data[2] << 8) | request->data[3]); // get number of registers to write
	num_bytes = request->data[4]; // get the number of bytes
	if (((register_quantity < 1) | (register_quantity > 0x007D)) & (num_bytes = (register_quantity<<2)))
	{
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_ILLEGAL_DATA_VALUE;
		response->len = 1;
		return MB_ILLEGAL_DATA_VALUE;
	}
	
	start_address = ((request->data[0] << 8) | request->data[1]); // get start address
	end_address = start_address + register_quantity; // calculate end address
	
	// if end address is greater than max or we're dealing with an illegal address
	if (end_address > mb->numHoldingRegs || illegalModbusAddress(mb, start_address))
	{
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_ILLEGAL_DATA_ADDRESS;
		response->len = 1;
		return MB_ILLEGAL_DATA_ADDRESS;
	}
	
	// copy data over to holding registers
	data_index = 5; // Data starts at the 5th index
	for (index=start_address; index<end_address; index++)
	{
		mb->holdingRegisters[index] = (request->data[data_index] << 8) | (request->data[data_index+1]);
		data_index = data_index + 2;
	}
	
	// Perform the action for that address from main application
	return_code = mb->modbusActionFunction(start_address, WRITE_MULTIPLE_REGISTER);
	
	// If the action returns an error, send response
	if (return_code != MB_NO_ERROR)
	{
		if(mb->errorRegEnable) {
			mb->holdingRegisters[mb->errorRegAddr] = return_code;
		}
		response->function_code = 0x80 + request->function_code;
		response->data[0] = MB_SLAVE_DEVICE_FAILURE;
		response->len = 1;
		return MB_SLAVE_DEVICE_FAILURE;
	}

	// function code
	response->function_code = 	request->function_code;
	// start address
	response->data[0] = request->data[0];
	response->data[1] = request->data[1];
	// Quantity
	response->data[2] = request->data[2];
	response->data[3] = request->data[3];	
	response->len = 4;

	
	return return_code;
}

/*
	processModbusRequest
	Directs a packet to its respective function code process
	
	mb - Pointer to the main modbus object
*/
Modbus_RTU_Error processModbusRequest(Modbus_Protocol *mb) {
	uint8_t return_code = MB_NO_ERROR;
	switch(mb->modbus_request.modbus_pdu.function_code) {
		case READ_HOLDING_REGISTER:
			return_code = readHoldingRegisters(mb);
			break;
		case WRITE_SINGLE_REGISTER:
			return_code = writeSingleRegister(mb);
			break;
		case WRITE_MULTIPLE_REGISTER:
			return_code = writeMultipleRegister(mb);
			break;
		default:
			mb->modbus_response.modbus_pdu.function_code = 0x80 + mb->modbus_request.modbus_pdu.function_code;
			mb->modbus_response.modbus_pdu.data[0] = MB_ILLEGAL_FUNCTION;
            mb->modbus_response.modbus_pdu.len=1;
			return_code = MB_ILLEGAL_FUNCTION;
	}
	return return_code;
}
/*
	modbusHandler
	Function to handle a recieved modbus packet. Will take in the data, process it, make sure that it
	is a valid packet, and then enact the command
	
	mb - Pointer to the main modbus object
*/
void modbusHandler(Modbus_Protocol *mb) {
	uint16_t i;
	uint8_t request_ok;
	mb->txOk = false;
  
	request_ok = modbusRequestAdu(mb); // check request and move the data from rx_data array
	if(request_ok == 1) { // if request is for this RTU and crc is okay
		processModbusRequest(mb); // process the request
		if(mb->modbus_request.addr != 0) { // if this isn't a broadcast we can respond
			if(mb->groupAddrEnable) { // if group address is enabled
				if (mb->modbus_request.addr == mb->groupAddr) { // if the request addresses group address
					return; // kick out
				}
			}
			mb->modbus_response.addr = mb->modbusAddr; // set response modbus address
			mb->bytes_to_send = modbusResponseAdu(mb); // create the modbus response
            mb->txOk = true;
		}
	}
}
