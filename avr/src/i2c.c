/*
 * Master mode i2c
 *
 * Jakub Kaderka 2016
 */

#include <avr/io.h>
#include <inttypes.h>

#ifdef USICR
	#define I2C_USI
	#include "usi_twi.h"
#else
	#include "twi.h"
#endif

void i2c_init(void)
{
#ifdef I2C_USI
	USI_TWI_init();
#else
	TWI_init();
#endif
}

void i2c_write(uint8_t address, uint8_t *buffer, uint8_t length)
{
#ifdef I2C_USI
	USI_TWI_rw(address, buffer, length);
#else
	TWI_rw(address, buffer, length);
#endif
}

void i2c_read(uint8_t address, uint8_t *buffer, uint8_t length)
{
#ifdef I2C_USI
	USI_TWI_rw(address|1, buffer, length);
#else
	TWI_rw(address|1, buffer, length);
#endif
}
