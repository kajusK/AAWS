/*
 * Master mode TWI using hw TWI module
 *
 * http://www.atmel.com/images/doc2564.pdf
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#include <avr/io.h>
#include <inttypes.h>
#include <util/twi.h>

//TODO rewrite, definition which part to compile should be in Makefile
#ifdef TWSR

#include "config.h"

#undef TW_START		//defined in library, not needed here, the name is great
#define TW_START (_BV(TWINT) | _BV(TWEN) | _BV(TWSTA))
#define TW_STOP (_BV(TWINT) | _BV(TWEN) | _BV(TWSTO))
#define TW_DATA (_BV(TWINT) | _BV(TWEN))

#ifdef TWI_MODE_FAST
	#define TW_FREQ 4
#else
	#define TW_FREQ 1
#endif

#define TWSR_VALUE 0
#if F_CPU < 3600000UL
	//check datasheet, we can't run full speed on MCUs under 3,6MHz
	#define TWBR_VALUE 10
	#warning "I2C will be running under 100kHz due to slow MCU clock"
#elif F_CPU < 14400000UL && defined TWI_FAST_MODE
	#define TWBR_VALUE 10
	#warning "I2C will be running under 400kHz due to slow MCU clock"
#else
	#define TWBR_VALUE (F_CPU/100000UL - 16*TW_FREQ)/(TW_FREQ*2)
#endif

/*
 * send selected condition
 */
static uint8_t TWI_transmit(uint8_t mask)
{
	TWCR = mask;
	if (mask == TW_STOP)
		return 0; //TWINT not modified when sending stop condition
	loop_until_bit_is_set(TWCR, TWINT);
	return TW_STATUS;
}

/*
 * Read length bytes into buffer
 */
static void TWI_read(uint8_t *buffer, uint8_t length)
{
	do {
		if (length == 1)
			TWI_transmit(TW_DATA);	//nack
		else
			TWI_transmit(TW_DATA | _BV(TWEA));
		*buffer++ = TWDR;
	} while (--length > 0);
}

/*
 * Write buffer into device, return 1 if nack received
 */
static uint8_t TWI_write(uint8_t *buffer, uint8_t length)
{
	do {
		TWDR = *buffer++;
		if (TWI_transmit(TW_DATA) != TW_MT_DATA_ACK)
			return 1;
	} while (--length > 0);

	return 0;
}

static uint8_t TWI_send_addr(uint8_t address)
{
	TWDR = address;
	uint8_t status = TWI_transmit(TW_DATA);

	if (address & 0x01) {
		if (status != TW_MR_SLA_ACK)
			return 1;
	} else if (status != TW_MT_SLA_ACK) {
		return 1;
	}

	return 0;
}

/*
 * Write/read data from/to buffer from device with given address
 *
 * Read or write mode is determined from the device address (i2c standard)
 */
uint8_t TWI_rw(uint8_t address, uint8_t *buffer, uint8_t length)
{
	uint8_t res = 0;

	TWI_transmit(TW_START);

	if (TWI_send_addr(address) != 0) {
		TWI_transmit(TW_STOP);
		return 1;			//no ack arrived
	}

	if (address & 1)
		TWI_read(buffer, length);
	else
		res = TWI_write(buffer, length);

	TWI_transmit(TW_STOP);
	return res;
}

/*
 * Basic initialization, single master mode
 */
void TWI_init(void)
{
	//set clock
	TWSR = TWSR_VALUE;
	TWBR = TWBR_VALUE;
}

#endif
