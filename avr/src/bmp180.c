/*
 * BMP180 temperature and pressure sensor
 *
 * -40 to 85 (+-2) °C
 * 300 to 1100 (+-6) hPa
 *
 * https://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
 *
 * Jakub Kaderka 2016
 */

#include <inttypes.h>
#include <util/delay.h>

#include "config.h"
#include "bmp180.h"
#include "i2c.h"

#define BMP180_ADDR		0x77

#define BMP180_REG_CHIP_ID	0xd0
#define BMP180_CHIP_ID		0x55

#define BMP180_CMD_TEMP		0x2e	//4,5 ms
#define BMP180_CMD_PRES		0x34	//timing depends on OSS - 4,5 to 25,5 ms

#define BMP180_REG_CONTROL	0xf4
#define BMP180_REG_DATA		0xf6

static int16_t ac1;
static int16_t ac2;
static int16_t ac3;
static uint16_t ac4;
static uint16_t ac5;
static uint16_t ac6;
static int16_t b1;
static int16_t b2;
static int16_t mb;
static int16_t mc;
static int16_t md;

static uint8_t bmp180_read_reg(uint8_t address)
{
	uint8_t buffer;

	if (i2c_write(BMP180_ADDR, &address, 1) != 0)
		return 0;
	if (i2c_read(BMP180_ADDR, &buffer, 1) != 0)
		return 0;
	return buffer;
}

static uint16_t bmp180_read_reg16(uint8_t address)
{
	uint16_t buffer;

	if (i2c_write(BMP180_ADDR, &address, 1) != 0)
		return 0;
	if (i2c_read(BMP180_ADDR, (uint8_t *)&buffer, 2) != 0)
		return 0;

	//swap bytes to match MSB LSB order
	return (buffer >> 8) | (buffer << 8);
}

static void bmp180_write(uint8_t address, uint8_t data)
{
	uint8_t buffer[2];
	buffer[0] = address;
	buffer[1] = data;

	i2c_write(BMP180_ADDR, buffer, 2);
}

static uint16_t bmp180_read_UT(void)
{
	//start measurement
	bmp180_write(BMP180_REG_CONTROL, BMP180_CMD_TEMP);
	//wait for results
	_delay_ms(4.5);
	return bmp180_read_reg16(BMP180_REG_DATA);
}

static uint32_t bmp180_read_UP(void)
{
	uint32_t UP;
	bmp180_write(BMP180_REG_CONTROL, BMP180_CMD_PRES + (BMP180_OSS << 6));
	//slightly longer than needed, but works for all OSS values
	_delay_ms(3*(BMP180_OSS*BMP180_OSS)+4.5);

	UP = (uint32_t) bmp180_read_reg16(BMP180_REG_DATA) << 8;
	UP |= bmp180_read_reg(BMP180_REG_DATA+2);
	UP >>= (8 - BMP180_OSS);

	return UP;
}

/*
 * Read configuration data from device, must be called before reading pressure
 */
uint8_t bmp180_init(void)
{
	i2c_init();
	// check if the chip communicates - ID is defined in datasheet
	if (bmp180_read_reg(BMP180_REG_CHIP_ID) != BMP180_CHIP_ID)
		return 1;

	ac1 = bmp180_read_reg16(0xAA);
	ac2 = bmp180_read_reg16(0xAC);
	ac3 = bmp180_read_reg16(0xAE);
	ac4 = bmp180_read_reg16(0xB0);
	ac5 = bmp180_read_reg16(0xB2);
	ac6 = bmp180_read_reg16(0xB4);
	b1 = bmp180_read_reg16(0xB6);
	b2 = bmp180_read_reg16(0xB8);
	mb = bmp180_read_reg16(0xBA);
	mc = bmp180_read_reg16(0xBC);
	md = bmp180_read_reg16(0xBE);

	return 0;
}

/*
 * Read the temperature (in tenths of °C) and pressure (in Pascals)
 */
uint8_t bmp180_read(int16_t *temp, uint32_t *pressure)
{
	int32_t B3, B5, B6, X1, X2, X3, p;
	uint32_t B4, B7, UP, UT;

	UT = bmp180_read_UT();
	UP = bmp180_read_UP();
	if (UP == 0)
		return 1;

	// calculate temperature
	X1 = ((UT - (int32_t)ac6) * ((int32_t)ac5)) >> 15;
	X2 = ((int32_t)mc << 11) / (X1 + (int32_t)md);
	B5 = X1 + X2;
	*temp = (B5 + 8) >> 4;

	// calculate pressure
	B6 = B5 - 4000;
	X1 = ((int32_t)b2 * ((B6 * B6) >> 12)) >> 11;
	X2 = ((int32_t)ac2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = ((((int32_t)ac1 * 4 + X3) << BMP180_OSS) + 2 ) >> 2;
	X1 = ((int32_t)ac3 * B6) >> 13;
	X2 = ((int32_t) b1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = ((uint32_t)ac4 * (uint32_t)(X3 + 32768)) >> 15;
	B7 = (UP - B3) * (uint32_t)(50000UL >> BMP180_OSS);

	if (B7 < 0x80000000)
		p = (B7 << 1) / B4;
	else
		p = (B7 / B4) << 1;

	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;
	p = p + ((X1 + X2 + (int32_t)3791)>>4);

	*pressure = p;

	return 0;
}
