/*
 * Master mode i2c
 *
 * Jakub Kaderka 2016
 *
 * TODO: Write lib for TWI module and create automatic switch (define) between
 * 	TWI and USI modules based on used avr
 */

#include <inttypes.h>
#include "usi_twi.h"

void i2c_init(void)
{
	USI_TWI_init();
}

void i2c_write(uint8_t address, uint8_t *buffer, uint8_t length)
{
	USI_TWI_rw(address, buffer, length);
}

void i2c_read(uint8_t address, uint8_t *buffer, uint8_t length)
{
	USI_TWI_rw(address|1, buffer, length);

}
