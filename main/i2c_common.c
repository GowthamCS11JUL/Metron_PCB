#include "headers.h"
#include "variables.h"






/* =========================================================
 * LOCAL BUFFERS
 * ========================================================= */
static uint8_t verify_data[16];
static uint8_t tx_buffer[16];

/* =========================================================
 * I2C0 INIT
 * ========================================================= */
void i2c0_init(void) {
  I2C_Controller_Initialize(&Daughter_I2C, Daughter_I2C_INST, 0);
}

/* =========================================================
 * I2C1 INIT
 * ========================================================= */
void i2c1_init(void) {
  I2C_Controller_Initialize(&Master_MCU, Master_I2C_INST, 1);
}

/* =========================================================
 * TIMEOUT WAIT
 * ========================================================= */
static bool I2C_Wait_With_Timeout(I2C_Controller *controller) {
  uint32_t start_time;
  uint32_t timeout_us = 500000;

  start_time = Delay_Timer_Get(&USB_BUFFER);

  while (!I2C_Controller_Transaction_Done(controller)) {
    if ((Delay_Timer_Get(&USB_BUFFER) - start_time) >= timeout_us) {
      controller->state = I2C_CONTROLLER_IDLE;
      return false;
    }
  }

  if (controller->error != I2C_CONTROLLER_ERROR_NONE) {
    return false;
  }

  return true;
}

/* =========================================================
 * I2C WRITE
 * ========================================================= */
int32_t i2c_write(I2C_Controller *controller, uint8_t slave_address,
                  uint8_t *data, uint8_t length) {
  if (controller == NULL)
    return -1;

  if (data == NULL)
    return -2;

  if (length == 0)
    return -3;

  if (length > sizeof(tx_buffer))
    return -4;

  if (controller->state != I2C_CONTROLLER_IDLE)
    return -5;

  memcpy(tx_buffer, data, length);

  controller->error = I2C_CONTROLLER_ERROR_NONE;

  I2C_Controller_Start_Transmit(controller, slave_address, tx_buffer, length);

  if (!I2C_Wait_With_Timeout(controller))
    return -6;

  return 1;
}

/* =========================================================
 * I2C READ
 * ========================================================= */
int32_t i2c_read(I2C_Controller *controller, uint8_t slave_address,
                 uint8_t reg_address, uint8_t *rx_buffer, uint8_t length) {
  if (controller == NULL)
    return -1;

  if (rx_buffer == NULL)
    return -2;

  if (length == 0)
    return -3;

  if (length > sizeof(verify_data))
    return -4;

  if (controller->state != I2C_CONTROLLER_IDLE)
    return -5;

  /* Send register address */
  controller->error = I2C_CONTROLLER_ERROR_NONE;

  I2C_Controller_Start_Transmit(controller, slave_address, &reg_address, 1);

  if (!I2C_Wait_With_Timeout(controller))
    return -6;

  /* Receive data */
  controller->error = I2C_CONTROLLER_ERROR_NONE;

  I2C_Controller_Start_Receive(controller, slave_address, verify_data, length);

  if (!I2C_Wait_With_Timeout(controller))
    return -7;

  memcpy(rx_buffer, verify_data, length);

  return 1;
}

/* =========================================================
 * I2C WRITE VERIFY
 * ========================================================= */
int32_t i2c_write_verify(I2C_Controller *controller, uint8_t slave_address,
                         uint8_t *data, uint8_t length) {
  uint8_t reg_address;

  if (controller == NULL)
    return -1;

  if (data == NULL)
    return -2;

  if (length < 2)
    return -3;

  reg_address = data[0];

  /* Write */
  if (i2c_write(controller, slave_address, data, length) != 1) {
    return -4;
  }

  /* Read Back */
  if (i2c_read(controller, slave_address, reg_address, verify_data,
               length - 1) != 1) {
    return -5;
  }

  /* Compare */
  if (memcmp(&data[1], verify_data, length - 1) != 0) {
    return 0;
  }

  return 1;
}