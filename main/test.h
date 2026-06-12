#ifndef TEST_H_
#define TEST_H_

#include "usb.h"
#include "dynamic_parser.h"

/* =========================================================
 * MAIN TEST COMMAND HANDLER
 * ========================================================= */

void test_command_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

/* =========================================================
 * SUB HANDLERS
 * ========================================================= */

void usb_test_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

void uart_test_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);


#endif /* TEST_H_ */