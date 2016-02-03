/* definitions to keep the things sane, don't modify */
#define PORT_(port)	PORT ## port
#define DDR_(port)	DDR  ## port
#define PIN_(port)	PIN  ## port

#define PORT(port)	PORT_(port)
#define DDR(port)	DDR_(port)
#define PIN(port)	PIN_(port)

#define VERSION	"0.1"

// serial baudrate
#define BAUD	9600

/*
 * Wind speed calibration
 *
 * Take several measurements of wind speed (e.g. by car with wind sensor mounted
 * on roof) and calculate correction:
 *
 * Some equation
 */
#define WIND_CORRECTION 1234

/*
 * Rain calibration
 *
 * Pour known amount of water (e.g. equal to 20mm of rainfalls) to your
 * rain gauge and measure a number of pulses generated by the tipping bucket.
 *
 * Calculate pulses per mm and round to hundreds of mm
 * mm_per_pulse = used_water / number_of_pulses
 */
#define RAIN_MM_PER_PULSE 10

/*
 * IO definitions
 */
// dht22 - humidity and temp
#define DHT22_PORT	D
#define DHT22		6

//wind dir, requires 5 pins in a row, define first pin and gate, the defined
//pin and folowing 4 will be used, e.g. 2 means pins 2,3,4,5,6 will be used
#define WIND_DIR_PORT	A
#define WIND_DIR_START	0

//rain sensor is using pin associated with INT0
//wind speed sensor is using pin ICP1
