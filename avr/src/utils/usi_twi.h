/*
 * Master mode TWI using USI module
 *
 * Jakub Kaderka 2016
 */

#ifndef _USI_TWI_H_
#define _USI_TWI_H_

//uncomment to use fast 400khz transmission, 100khz by default
//#define TWI_MODE_FAST

/*
 * Write/read data from/to buffer from device with given address
 *
 * Read or write mode is determined from the device address (i2c standard)
 */
uint8_t USI_TWI_rw(uint8_t address, uint8_t *buffer, uint8_t length);

/*
 * Basic initialization, single master mode
 */
void USI_TWI_init(void);

#endif
