;**********************************************************************
; Weather Station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; weather.inc
;**********************************************************************
;---------------constants---------------------
BAUDRATE	equ	25	;uart baud rate 9.2k, brgh = 1

;number of errors after which the device is considered broken
ERR_TO_FAIL	equ	10

;bmp180
BMP_WRITE_ADDR	equ	0xEF	;i2c write addr
BMP_READ_ADDR	equ	0xEE	;i2c read add
BMP_CONF_REG	equ	0xF4	;config register addres, store 0x2e to read temp, 0xb4 to read double oversampling pressure
BMP_COEF_BASE	equ	0xaa	;address of the first calibration coeficient
BMP_COEF_NUM	equ	22	;number of coeficients to read
BMP_DATA_BASE	equ	0xf6	;MSB of the result address
BMP_TEMP_CMD	equ	0x2e	;command to read temperature
BMP_PRESR_CMD	equ	0xb4	;command to read pressure with high resolution

BMP_CAL_BASE	equ	0xa0	;inside pics memory

;data space to temporary store received data
DATA_SPACE_BASE	equ	0x20

;info register
ack		equ	0
bmp180_set	equ	1	;1 if the pressure sensor was initialized
temp_hum_ready	equ	2	;1 if temperature and humidity are in memory
pressure_ready	equ	3	;1 if pressure data are valid
cmd_start	equ	4	;previous received byte was #
decimal		equ	7	;1 if we take the number as fixed point decimal
;----------------------pins-------------------
#define	dht22	portb, 7
#define sda	portb, 4
#define scl	portb, 5
;**********************************************************************
;variables
;**********************************************************************
	cblock	0x20
; bank0, 80bytes
;------------------------
;0x20 - 0x24 for data to receive/write, convert...
;DATA_SPACE_BASE
	data1
	data2
	data3
	data4
	data5
;.....................
;used during communication
	count_bytes
	checksum

;for table calculations
	table_offset
	char_count

;all the data collected
;order matters! it is expected in bin2bcd
;///////
;valid when info, temp_hum_ready is 1
	temp_h
	temp_l		;temperature, 16 bit, one decimal place, +-0,5 °C
	humidity_h
	humidity_l	;humidity, 16 bit, one decimal place, in rh %, +-2%
;valid when info, pressure_ready is 1
	pressure_h
	pressure_l	;pressure, 16 bit, in Pa, +-4 Pa
;valid all the time
	rain_h
	rain_l		;rainfalls, 16 bin, one decimal place in mm/m^2, +-0,1mm

	wind_speed_h
	wind_speed_l	;wind speed, 16 bit, one decimal place, in m/s, +-0,1m/s

	wind_dir_h	; ->|
	wind_dir_l	; ->|-needed for number conversion, order matters
;//////
	wind_dir

	wind_cnt_h	; -|
	wind_cnt_l	; -|
	rain_cnt_h	; -|
	rain_cnt_l	; -|> needed to avoid disabling interrupt in send_data

;errors from last successful getting data
	dht22_err
	bmp180_err
;------------------------
	endc

;bank 1
	cblock 0xa0
;-----------------------
;0xa0 - 0xb6 for air pressure calibration data
;BMP_CAL_BASE
	ac1_h
	ac1_l
	ac2_h
	ac2_l
	ac3_h
	ac3_l
	ac4_h
	ac4_l
	ac5_h
        ac5_l
	ac6_h
	ac6_l

	b1_h
	b1_l
	b2_h
	b2_l
	mb_h
	mb_l
	mc_h
	mc_l
	md_h
	md_l
;......................

	i2c_data
;----------------------
	endc

	cblock	0x70
;mirrored into all banks, 16bytes
;------------------------
	w_sav
	stat_sav
	fsr_sav

	info

	wait1
	wait2

	;needs to be here, i2c works mostly in bank1 and uses this
	count_bits
;------------------------
	endc
