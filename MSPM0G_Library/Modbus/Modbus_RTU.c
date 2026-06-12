#include <string.h>
#include <ti/driverlib/driverlib.h>
#include <UART_Interface.h>
#include <Timer_Timeout.h>
#include <UART_Target.h>
#include "Modbus_Protocol.h"
#include <Modbus_RTU.h>

uint8_t Modbus_RTU_Action_Function(uint16_t addr, uint8_t rtuFunction) {
    uint16_t ii, index;
    uint8_t errCode=MB_ILLEGAL_DATA_ADDRESS;
    for (ii=0; ii<glModbusRtu.numHoldingRegs; ii++) {
        if (addr == glModbusRtu.holdingRegInfo[ii].address){
            errCode = glModbusRtu.holdingRegInfo[ii].callBack(addr, rtuFunction);
            break;
    }
           
    }
    
    return errCode;
}

void Modbus_RTU_Initialize(Modbus_RTU *rtu, Modbus_Protocol_Config *config,  Modbus_RTU_Register *rtuRegs, uint8_t uartTargetIndex, uint16_t maxNumRx) {
    rtu->modbus.groupAddrEnable = config->rtuGroupAddressEnable;
    rtu->modbus.modbusAddr =  config->rtuAddress;
    rtu->modbus.groupAddr = config->rtuGroupAddress;
    rtu->modbus.rx_data = config->rxData;
    rtu->modbus.tx_data = config->txData;
    rtu->modbus.modbus_request.modbus_pdu.data = config->requestAduData;
    rtu->modbus.modbus_response.modbus_pdu.data = config->responseAduData;
    rtu->modbus.numIllegalRegs = config->numIllegalRegs;
    rtu->modbus.numHoldingRegs = config->numHoldingRegs;
    rtu->numHoldingRegs = config->numHoldingRegs;
    rtu->modbus.illegalModbusAddresses = config->illegalRegs;
    rtu->modbus.holdingRegisters = config->holdingRegs;
     rtu->holdingRegs =  config->holdingRegs;  
    rtu->modbus.errorRegEnable = config->errorRegEnable;
    rtu->modbus.errorRegAddr = config->errorRegAddress;
    rtu->numRx = 0;
    rtu->numTx = 0;
    rtu->uartTargetIndex = uartTargetIndex;
    rtu->maxNumRx = maxNumRx;
    rtu->holdingRegInfo = rtuRegs;
    rtu->modbus.modbusActionFunction = Modbus_RTU_Action_Function;
}


uint32_t Modbus_RTU_Silent_Interval(uint32_t baudRate) {
    return modbusSilentInterval(baudRate);
}

void Modbus_RTU_FSM(Modbus_RTU *rtu)
{
    UART_Target_State uartTargetState;
    uartTargetState = UART_Target_getState(rtu->uartTargetIndex);
    switch(uartTargetState) {
        case (UART_TARGET_IDLE):
           
            UART_Target_Receive(rtu->uartTargetIndex, rtu->modbus.rx_data, rtu->maxNumRx);
            rtu->numRx = rtu->modbus.modbus_received_bytes;
            break;
        case (UART_TARGET_WAITING):
        case (UART_TARGET_RECEIVING):
            __NOP();
            break;
        case (UART_TARGET_RECEIVED):
            rtu->modbus.modbus_received_bytes=UART_Interface_NumRX(glUartTarget[rtu->uartTargetIndex].uart);
            
            memset((uint8_t *)rtu->modbus.tx_data, 0,  rtu->numTx);
             rtu->modbus.bytes_to_send=0;            
            modbusHandler(&rtu->modbus);
            memset((uint8_t *)rtu->modbus.rx_data, 0,  rtu->maxNumRx);

            if(rtu->modbus.bytes_to_send){
            rtu->numTx = rtu->modbus.bytes_to_send;
            UART_Target_Transmit(rtu->uartTargetIndex, rtu->modbus.tx_data, rtu->numTx);
            }
            else {
                 UART_Target_setState(rtu->uartTargetIndex,UART_TARGET_IDLE);
            }

        case (UART_TARGET_SENDING):
            __NOP();
            break;
        default:
            break;
    }
}

void Modbus_RTU_Add_Register(Modbus_RTU *rtu, uint16_t index, uint16_t address, uint16_t length, uint8_t (*callBack) (uint16_t, uint8_t)) {

    rtu->holdingRegInfo[index].address = address;
    rtu->holdingRegInfo[index].callBack = callBack;
    rtu->holdingRegInfo[index].length = length;
}

uint16_t Modbus_RTU_Get_Register(Modbus_RTU *rtu, uint16_t address, uint16_t *data ) {
    uint16_t ii, index;
    for (ii=0; ii<rtu->numHoldingRegs; ii++) {
        if (address == rtu->holdingRegInfo[ii].address)
            break;
    }
    index = ii;
    for (ii=0; ii < rtu->holdingRegInfo[index].length; ii++)
        data[ii] = rtu->holdingRegs[rtu->holdingRegInfo[index].address + ii];
    return index;
}