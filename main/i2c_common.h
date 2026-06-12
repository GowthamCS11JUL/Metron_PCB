/*
 * i2c_common.h
 */

#ifndef I2C_COMMON_H_
#define I2C_COMMON_H_

#include <stdbool.h>
#include <stdint.h>


#include "I2C_Controller.h"

/* =========================================================
 * INIT FUNCTIONS
 * ========================================================= */
void i2c0_init(void);
void i2c1_init(void);

/* =========================================================
 * I2C WRITE
 *
 * data format:
 * [REGISTER][DATA...]
 *
 * Example:
 * {0x10, 0x55}
 * ========================================================= */
int32_t i2c_write(I2C_Controller *controller, uint8_t slave_address,
                  uint8_t *data, uint8_t length);

/* =========================================================
 * I2C READ
 *
 * reg_address = register to read
 * rx_buffer   = output buffer
 * length      = number of bytes
 * ========================================================= */
int32_t i2c_read(I2C_Controller *controller, uint8_t slave_address,
                 uint8_t reg_address, uint8_t *rx_buffer, uint8_t length);

/* =========================================================
 * I2C WRITE VERIFY
 *
 * Writes data then reads back and compares
 *
 * data format:
 * [REGISTER][DATA...]
 * ========================================================= */
int32_t i2c_write_verify(I2C_Controller *controller, uint8_t slave_address,
                         uint8_t *data, uint8_t length);

#endif /* I2C_COMMON_H_ */