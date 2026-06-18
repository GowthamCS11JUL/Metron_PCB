#ifndef APP_H_
#define APP_H_

void app_init(void);
void cmd_data_setup(void);
void command_dispatcher( UART_DATA *usb_data, Parsed_Command_t *cmd );

#endif