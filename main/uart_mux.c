#include "headers.h"
#define RX_BUFFER_SIZE 64

/*=========================================================
 * GLOBALS
 *=========================================================*/







/*=========================================================
 * UART MUX GPIO INIT
 *=========================================================*/

void uart_pin_init(void) {
  DL_GPIO_initDigitalOutput(UART_En_IOMUX);
  DL_GPIO_enableOutput(UART_En_PORT, UART_En_PIN);

  DL_GPIO_initDigitalOutput(UART_Select_A1_IOMUX);
  DL_GPIO_enableOutput(UART_Select_A1_PORT, UART_Select_A1_PIN);

  DL_GPIO_initDigitalOutput(UART_Select_A0_IOMUX);

  DL_GPIO_enableOutput(UART_Select_A0_PORT, UART_Select_A0_PIN);

  DL_GPIO_clearPins(UART_En_PORT, UART_En_PIN);

  DL_GPIO_clearPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

  DL_GPIO_clearPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
}

/*=========================================================
 * LOOPBACK TEST
 *=========================================================*/

uint8_t uart_loopback_start(void)
 {
     char rxBuffer[RX_BUFFER_SIZE];
  uint8_t index = 0;
   while (1) {
    if (!DL_UART_isRXFIFOEmpty(UART3)) {
      char ch = DL_UART_receiveData(UART3);

      /* End of command */
      if ((ch == '\r') || (ch == '\n')) {
        rxBuffer[index] = '\0';

        /* Exit command */
        if (strcmp(rxBuffer, "EXIT") == 0) {
          break;
        }

        /* Test command */
        if (strcmp(rxBuffer, "This to test the UART line of site") == 0) {
          const char *msg = "TRUE\r\n";

          while (*msg) {
            while (DL_UART_isTXFIFOFull(UART3))
              ;
            DL_UART_transmitData(UART3, *msg++);
          }
        } else {
          const char *msg = "FALSE\r\n";
           
          while (*msg) {
            while (DL_UART_isTXFIFOFull(UART3))
              ;
            DL_UART_transmitData(UART3, *msg++);
          }
          return 0;
        }

        index = 0;
        memset(rxBuffer, 0, sizeof(rxBuffer));
      } else {
        if (index < (RX_BUFFER_SIZE - 1)) {
          rxBuffer[index++] = ch;
        }
      }
    }
  }
return 1;
}

/*=========================================================
 * UART VMUX CHANNEL SELECT
 *=========================================================*/

uint8_t uart_vmux_select(uart_channel ch) {
  uint8_t result;

    DL_GPIO_setPins(UART_En_PORT, UART_En_PIN);

    switch (ch) {
    case UART_CH0:

      DL_GPIO_clearPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

      DL_GPIO_clearPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
      break;

    case UART_CH1:

      DL_GPIO_clearPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

      DL_GPIO_setPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
      break;

    case UART_CH2:

      DL_GPIO_setPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

      DL_GPIO_clearPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
      break;

    case UART_CH3:

      DL_GPIO_setPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

      DL_GPIO_setPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
      break;

    default:

      DL_GPIO_clearPins(UART_En_PORT, UART_En_PIN);

      strcpy((char *)executing_command->response, "FAIL");

      executing_command->resp_len = 4;

      return 0;
    }

  /* Allow MUX to settle */
  delay_cycles(32000); /* 1 ms @ 32 MHz */

  result = uart_loopback_start();
  return result;
}

/*=========================================================
 * UART RMUX CHANNEL SELECT
 *=========================================================*/

void uart_rmux_select(uart_channel ch) {

  DL_GPIO_setPins(UART_En_PORT, UART_En_PIN);

  switch (ch) {
  case UART_CH0:

    DL_GPIO_clearPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

    DL_GPIO_clearPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
    break;

  case UART_CH1:

    DL_GPIO_clearPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

    DL_GPIO_setPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
    break;

  case UART_CH2:

    DL_GPIO_setPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

    DL_GPIO_clearPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
    break;

  case UART_CH3:

    DL_GPIO_setPins(UART_Select_A1_PORT, UART_Select_A1_PIN);

    DL_GPIO_setPins(UART_Select_A0_PORT, UART_Select_A0_PIN);
    break;

  default:

    return;
  }

  /* Allow MUX to settle */
  delay_cycles(32000); /* 1 ms @ 32 MHz */
}