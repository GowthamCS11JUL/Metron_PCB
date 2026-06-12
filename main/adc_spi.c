#include "headers.h"
#include "ti/driverlib/m0p/dl_core.h"

// Global storage
uint32_t adc_results[AD4115_NUM_CHANNELS] = {0};

// Forward Declarations for internal helper functions
static void AD4115BCPZ_RL7_config(void);
static void AD4115BCPZ_RL7_setup_config(void);

static void AD4115_SPI_Wait(void) {
  while (ADC.txState != SPI_IDLE)
    ;
  while (ADC.rxState != SPI_IDLE)
    ;
}

static void AD4115_Reset(void) {
  uint8_t tx[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  uint8_t rx[8];

  SPI_Transact_block(&ADC, tx, 8, rx, 8);

  AD4115_SPI_Wait();
  delay_cycles(32000);
}

void AD4115BCPZ_RL7_init(void) {
  SPI_Initialize_block_transfer(&ADC, SPI0, ADC_SPI_CS_PORT, 0, ADC_SPI_CS_PIN,
                                255, 255);
  DL_GPIO_initDigitalOutput(ADC_SPI_CS_IOMUX);
  DL_GPIO_enableOutput(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  AD4115_Reset();
  AD4115BCPZ_RL7_config();
}

static void AD4115BCPZ_RL7_config(void) {
  uint8_t tx_data[3];
  uint8_t rx_data[3] = {0};

  // Set DATA_STAT = 1
  tx_data[0] = 0x02;
  tx_data[1] = 0x00;
  tx_data[2] = 0x40;
  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  delay_cycles(800);
  // ADC Mode: Internal Ref, Single Conversion
  tx_data[0] = 0x01;
  tx_data[1] = 0x84;
  tx_data[2] = 0x10;
  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  delay_cycles(800);

  AD4115BCPZ_RL7_setup_config();
}

static void AD4115BCPZ_RL7_setup_config(void) {
  // Unipolar Mode (0x0320)
  uint8_t tx_data[3] = {0x20, 0x03, 0x20};
  uint8_t rx_data[3] = {0};
  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  delay_cycles(800);
}

uint32_t AD4115_ReadSingleChannel(uint8_t channel) {
  if (channel > 15)
    return 0;

  uint8_t tx_data[5];
  uint8_t rx_data[5];

  // Enable Channel
  uint16_t ch_config = (1U << 15) | (channel << 5) | 0x10;
  tx_data[0] = 0x10 + channel;
  tx_data[1] = (uint8_t)(ch_config >> 8);
  tx_data[2] = (uint8_t)(ch_config & 0xFF);

  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  delay_cycles(800);

  // Trigger Conversion
  tx_data[0] = 0x01;
  tx_data[1] = 0x84;
  tx_data[2] = 0x10;
  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  // Poll Status
  uint8_t status_tx[2] = {0x40, 0x00};
  uint8_t status_rx[2] = {0};

  uint32_t start = Delay_Timer_Get(&USB_BUFFER);

  do {
    DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
    SPI_Transact_block(&ADC, status_tx, 2, status_rx, 2);

    AD4115_SPI_Wait();
    DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

    if ((Delay_Timer_Get(&USB_BUFFER) - start) > 100000) {

      tx_data[0] = 0x10 + channel;
      tx_data[1] = 0x00;
      tx_data[2] = 0x00;
      DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
      SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
      AD4115_SPI_Wait();
      DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

      return 0;
    }

  } while ((status_rx[1] & 0x80U) != 0U);

  // Read Data
  tx_data[0] = 0x44;
  tx_data[1] = tx_data[2] = tx_data[3] = tx_data[4] = 0x00;
  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 5, rx_data, 5);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  uint8_t status_byte = rx_data[4];

  uint8_t actual_channel = status_byte & 0x0F;

  if (actual_channel != channel) {

    tx_data[0] = 0x10 + channel;
    tx_data[1] = 0x00;
    tx_data[2] = 0x00;
    DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
    SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
    AD4115_SPI_Wait();
    DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
    return 0;
  }
  uint32_t val =
      ((uint32_t)rx_data[1] << 16) | ((uint32_t)rx_data[2] << 8) | rx_data[3];

  // Disable Channel
  tx_data[0] = 0x10 + channel;
  tx_data[1] = 0;
  tx_data[2] = 0;
  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact_block(&ADC, tx_data, 3, rx_data, 3);
  AD4115_SPI_Wait();
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  return val;
}