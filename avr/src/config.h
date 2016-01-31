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
 * IO definitions
 */
// dht22 - humidity and temp
#define DHT22_PORT	D
#define DHT22		6

//wind dir, requires 5 pins in a row, define first pin and gate, the defined
//pin and folowing 4 will be used, e.g. 2 means pins 2,3,4,5,6 will be used
#define WIND_DIR_PORT	A
#define WIND_DIR_START	0
