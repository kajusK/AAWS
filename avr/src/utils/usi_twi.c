/*
 * Master mode TWI using USI module
 *
 * http://www.atmel.com/images/atmel-2561-using-the-usi-module-as-a-i2c-master_ap-note_avr310.pdf
 *
 * TODO: Rewrite waiting loops for pin change with some timeout to avoid
 * deadlocks
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <util/atomic.h>

//TODO rewrite, definition which part to compile should be in Makefile
#ifdef USIDR

#include "config.h"

#if defined(__AVR_AT90Mega169__) | defined(__AVR_ATmega169__) | \
	defined(__AVR_AT90Mega165__) | defined(__AVR_ATmega165__) | \
	defined(__AVR_ATmega325__) | defined(__AVR_ATmega3250__) | \
	defined(__AVR_ATmega645__) | defined(__AVR_ATmega6450__) | \
	defined(__AVR_ATmega329__) | defined(__AVR_ATmega3290__) | \
	defined(__AVR_ATmega649__) | defined(__AVR_ATmega6490__)

	#define SCL_PORT	E
	#define	SDA_PORT	E
	#define	SCL		4
	#define	SDA		5
#endif

#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | \
	defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
	#define SCL_PORT	B
	#define	SDA_PORT	B
	#define	SCL		2
	#define	SDA		0
#endif

#if defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__) | defined(__AVR_ATtiny4313__)
	#undef SCL_PORT		//surpress compiler warning of redefinition

	#define SCL_PORT	B
	#define	SDA_PORT	B
	#define	SCL		7
	#define	SDA		5
#endif

#define sda_high() { PORT(SDA_PORT) |= _BV(SDA); }
#define sda_low() { PORT(SDA_PORT) &= ~_BV(SDA); }

#define scl_high() { PORT(SCL_PORT) |= _BV(SCL); }
#define scl_low() { PORT(SCL_PORT) &= ~_BV(SCL); }
#define scl_state bit_is_set(PORT(SCL_PORT), SCL)

#ifdef TWI_MODE_FAST
	#define TWI_T1 0.6	//minimal high pulse length
	#define TWI_T2 1.3	//minimal low pulse length
#else
	#define TWI_T1 4
	#define TWI_T2 4.7
#endif

#define USI_TWI_8BIT (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)
#define USI_TWI_1BIT (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|(0xE<<USICNT0)

/*
 * i2c start condition
 */
void USI_TWI_start(void)
{
	sda_low();
	_delay_us(TWI_T1);
	scl_low();
}

/*
 * i2c stop condition
 */
void USI_TWI_stop(void)
{
	sda_low();
	scl_high();

	while (!scl_state)
		;			//wait for scl to be released by slave

	_delay_us(TWI_T1);
	sda_high();

	_delay_us(TWI_T2);		//wait for time required between starts
}

/*
 * read/write single byte
 *
 * data to write must be stored in USIDR before calling
 * returns received data
 */
static uint8_t USI_TWI_transfer(uint8_t temp)
{
	USISR = temp;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		do {
			_delay_us(TWI_T2);
			USICR |= _BV(USITC);	//scl high, data to/from port
			while (!scl_state)
				;		//wait for slave to release SCL
			_delay_us(TWI_T1);
			USICR |= _BV(USITC);	//scl back to low
		} while (!(USISR & _BV(USIOIF)));

		_delay_us(TWI_T2);
	}
	temp = USIDR;
	USIDR = 0xff;			//release sda
	DDR(SDA_PORT) |= _BV(SDA);

	return temp;
}

/*
 * Read length bytes into buffer
 */
static void USI_TWI_read(uint8_t *buffer, uint8_t length)
{
	do {
		DDR(SDA_PORT) &= ~_BV(SDA);	//sda to input
		*buffer++ =  USI_TWI_transfer(USI_TWI_8BIT);

		if (length == 1)
			USIDR = 0xff;	//end of transmission, send NACK
		else
			USIDR = 0;

		USI_TWI_transfer(USI_TWI_1BIT);
	} while (--length > 0);
}

/*
 * Write buffer into device, return 1 if nack received
 */
static uint8_t USI_TWI_write(uint8_t *buffer, uint8_t length)
{
	do {
		USIDR = *buffer++;
		USI_TWI_transfer(USI_TWI_8BIT);

		DDR(SDA_PORT) &= ~_BV(SDA);	//sda to input
		if (USI_TWI_transfer(USI_TWI_1BIT))
			return 1;	//nack arrived
	} while (--length > 0);

	return 0;
}

/*
 * Write/read data from/to buffer from device with given address
 *
 * Read or write mode is determined from the device address (i2c standard)
 */
uint8_t USI_TWI_rw(uint8_t address, uint8_t *buffer, uint8_t length)
{
	uint8_t res = 0;

	USI_TWI_start();

	if (!USI_TWI_write(&address, 1)) {
		USI_TWI_stop();
		return 1;			//no ack arrived
	}

	if (address & 1)
		USI_TWI_read(buffer, length);
	else
		res = USI_TWI_write(buffer, length);

	USI_TWI_stop();
	return res;
}

/*
 * Basic initialization, single master mode
 */
void USI_TWI_init(void)
{
	//preinitialize pins to avoid output set to low after switching DDR
	PORT(SCL_PORT) |= _BV(SCL);
	PORT(SDA_PORT) |= _BV(SDA);
	//set outputs
	DDR(SCL_PORT) |= _BV(SCL);
	DDR(SDA_PORT) |= _BV(SDA);

	//two wire mode, external clock for shift reg., software strobe (USITC)
	USICR = _BV(USIWM1)|_BV(USICS1)|_BV(USICLK);
}

#endif
