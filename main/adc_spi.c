
#include "headers.h"

uint32_t adc_results[AD4115_NUM_CHANNELS] = {0};

// Low-level helper function utilizing the software state-machine tracker
static inline void AD4115_Wait(SPI_Interface *spi) {
  while (spi->txState != SPI_IDLE) {
    // Wait for the asynchronous interrupt handler to finish the entire transaction
  }
}

static void AD4115_Reset(SPI_Interface *spi) {
  uint16_t tx[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
  uint16_t rx[4] = {0};

  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact(spi, tx, 4, rx, 4);
  AD4115_Wait(spi);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  delay_cycles(32000);
}

static void AD4115_WriteReg16(SPI_Interface *spi, uint8_t reg, uint16_t data) {
  uint16_t tx[2];
  uint16_t rx[2] = {0};

  tx[0] = ((uint16_t)reg << 8) | ((data >> 8) & 0xFF);
  tx[1] = ((data & 0xFF) << 8);

  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact(spi, tx, 2, rx, 2);
  AD4115_Wait(spi);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  
  delay_cycles(800);
}

static uint8_t AD4115_ReadStatus(SPI_Interface *spi) {
  uint16_t tx[1] = {(0x40 << 8)};
  uint16_t rx[1] = {0};

  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact(spi, tx, 1, rx, 1);
  AD4115_Wait(spi);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  return (uint8_t)(rx[0] & 0xFF);
}

uint16_t AD4115_ReadID(SPI_Interface *spi) {
  uint16_t id_tx[2] = {(0x47 << 8) | 0x00, 0x0000};
  uint16_t id_rx[2] = {0};

  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact(spi, id_tx, 2, id_rx, 2);
  AD4115_Wait(spi);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  uint16_t actual_id = ((id_rx[0] & 0x00FF) << 8) | (id_rx[1] >> 8);
  return actual_id;
}

void AD4115BCPZ_RL7_init(SPI_Interface *spi) {
  SPI_Initialize(spi, SPI0, 0);

  DL_GPIO_initDigitalOutput(ADC_SPI_CS_IOMUX);
  DL_GPIO_enableOutput(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  AD4115_Reset(spi);

  uint16_t id = AD4115_ReadID(spi);
  (void)id; // Suppress unused warning

  for (uint8_t ch = 0; ch < 16; ch++) {
    AD4115_WriteReg16(spi, 0x10 + ch, 0x0000);
  }

  // Set DATA_STAT = 1
  AD4115_WriteReg16(spi, 0x02, 0x0040);

  // Setup Config: True Unipolar Mode + Internal Ref
  AD4115_WriteReg16(spi, 0x20, 0x0320);

  // ADC Mode: Continuous/Single Conversion with Setup configs
  AD4115_WriteReg16(spi, 0x01, 0x8410);
}

uint32_t AD4115_ReadSingleChannel(uint8_t channel) {
  SPI_Interface *spi = &ADC;
  uint16_t chCfg;
  uint32_t start;
  uint8_t status;

  if (channel > 15) {
    return 0;
  }

  chCfg = (1U << 15) | ((uint16_t)channel << 5) | 0x10;
  AD4115_WriteReg16(spi, 0x10 + channel, chCfg);
  AD4115_WriteReg16(spi, 0x01, 0x8410);

  start = Delay_Timer_Get(&USB_BUFFER);
  do {
    status = AD4115_ReadStatus(spi);
    if ((Delay_Timer_Get(&USB_BUFFER) - start) > 100000) {
      AD4115_WriteReg16(spi, 0x10 + channel, 0x0000);
      return 0;
    }
  } while (status & 0x80);

  uint16_t tx[3] = {(0x44 << 8), 0x0000, 0x0000};
  uint16_t rx[3] = {0};

  DL_GPIO_clearPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);
  SPI_Transact(spi, tx, 3, rx, 3);
  AD4115_Wait(spi);
  DL_GPIO_setPins(ADC_SPI_CS_PORT, ADC_SPI_CS_PIN);

  uint8_t statusByte = (uint8_t)(rx[2] >> 8);
  uint8_t actualChannel = statusByte & 0x0F;

  // Reconstruct 24-bit ADC Value from the 16-bit word blocks
  uint32_t value = ((uint32_t)(rx[0] & 0xFF) << 16) |
                   ((uint32_t)(rx[1] >> 8) << 8)  | 
                   ((uint32_t)(rx[1] & 0xFF));

  // Turn off channel to clean up state
  AD4115_WriteReg16(spi, 0x10 + channel, 0x0000);

  if (actualChannel != channel) {
    return 0;
  }

  return value;
}