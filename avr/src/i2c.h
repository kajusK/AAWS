/*
 * Master mode i2c
 *
 * Jakub Kaderka 2016
 */

#ifndef _I2C_H_
#define _I2C_H_

void i2c_init(void);

void i2c_write(uint8_t address, uint8_t *buffer, uint8_t length);

void i2c_read(uint8_t address, uint8_t *buffer, uint8_t length);

#endif
