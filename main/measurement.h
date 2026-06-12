#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include "headers.h"

/* =========================================================
 * COMMAND HANDLER
 * ========================================================= */

void measurement_command_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

/* =========================================================
 * VOLTAGE COMMANDS
 * ========================================================= */

void voltage_single_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

void voltage_diff_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

void voltage_multiple_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

/* =========================================================
 * RESISTANCE COMMANDS
 * ========================================================= */

void resistance_handler(UART_DATA *usb_data, Parsed_Command_t *cmd);

#endif