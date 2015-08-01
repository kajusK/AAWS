;**********************************************************************
; Weather station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
;                                 PIC 16F88
;                         ------------\/-----------
;                      | -|RA2/AN2         RA1/AN1|- |
;      wind direction -| -|RA3/AN3         RA0/AN0|- | - wind direction
;                        -|RA4/T0CKI           RA7|-
;                        -|RA5/MCLR            RA6|-
;                    GND -|VSS                 VDD|- +5v
;	     Rain sensor -|RB0/INT             RB7|- humidity dht22
;                    RXD -|RB1/RX        RB6/T1CKI|- wind speed
;                    TXD -|RB2/TX              RB5|- I2C SCL |
;                        -|RB3/CCP1            RB4|- I2C SDA |- Air pressure
;                         -------------------------
;------------------------------
; Measured data are send to UART at 9600baud/s
;
; int wind_direction +- 23°
; float wind_speer +- 0.1 m.s-1
; int Pressure +- 4Pa
; float Temperature +- 0.5°C
; float Humidity +- 2% RH
; float Rain +- 0,1mm
;	ignores owerflow, needs to be flushed by sending '#R'
;	rain sensor should give a pulse every 0,1mm
;
;
; intenzita
; gamma zareni
; index uv
; snehova pokryvka
; znecisteni vzduchu
;

; Timer0 - rain sensor
; Timer1 - humidity sensor
;
;
; Rain sensor is connected to timer0 gate, thus incrementing it automatically
; without software intervention (except overflow)
;**********************************************************************
	LIST P=16F88, R=DEC
	include p16f88.inc
	errorlevel -302

	__CONFIG _CONFIG1, _CP_OFF & _CPD_OFF & _LVP_OFF & _BOREN_ON & _MCLRE_OFF & _PWRTE_ON & _WDT_ON & _INTRC_IO & _WRT_PROTECT_OFF & _DEBUG_OFF & _CCP1_RB3
	__CONFIG _CONFIG2, _IESO_OFF & _FCMEN_OFF

	include weather.inc

;**********************************************************************
; Beginning of the program execution
;**********************************************************************
	org 0x0
	goto init

	org 0x4
	goto interrupt
;######################################################################
; All the important parts
;######################################################################
general
	include tables.asm
	include routines.asm
	include math.asm
;communication
	include uart.asm
	include i2c.asm
;sensors
	include dht22.asm
	include bmp180.asm

;######################################################################
;interrupt
;
; Not critical for timing, used to read uart, i2c and handle
; overflow of rain timer and compare finished of wind sensor
;######################################################################
interrupt
;save registers values
	movwf	w_sav

	swapf	status, w
	movwf	stat_sav

	movf	fsr, w
	movwf	fsr_sav

	clrf	status
;-----------------------------
	btfsc	intcon, intf
	goto	int_rain
	btfsc	pir1, rcif
	goto	int_rx
	btfsc	intcon, t0if
	goto	int_tmr0
	goto	int_end		;TODO: should newer occur, clear unused flags
;.............................
;uart rx
;ignores everything except #R - reset rainfall measusement
int_rx
	call	uart_rx
	sublw	'#'		;command start
	btfss	status, z
	goto	int_rx2
	bsf	info, cmd_start	;next byte must be command or # again
	goto	int_end
int_rx2
	btfss	info, cmd_start
	goto	int_end		;missing the #

	bcf	info, cmd_start
	addlw	'#'		;get the received data again +#-# =0
	sublw	'R'
	btfss	status, z
	goto	int_end		;something different than R

	clrf	tmr0		;clear the timer
	clrf	rain_h
	clrf	rain_l		;and all the rain data
	bcf	intcon, t0if	;and just to be sure, also the timer int flag
	goto	int_end
;.............................
;measure wind speed, the tmr1 contains number of pulses from wind sensor
int_tmr0
	bcf	intcon, t0if

	bcf	t1con, tmr1on	;stop the timer 1 (externally incremented)
	;WARNING, ignoring he tmr1 overflow, seriously, the wind had to be
	;so fast it would destroy your house including the weather station
	movf	tmr1l, w
	movwf	wind_cnt_l
	movf	tmr1h, w
	movwf	wind_cnt_h	;save the number of pulses
	clrf	tmr1l
	clrf	tmr1h		;clear the counter
	clrf	tmr0		;and the timer
	bsf	t1con, tmr1on	;run the timer again
	;TODO add the calibration constant to timer
	goto	int_end
;.............................
; rain sensor
int_rain
	bcf	intcon, intf
	incfsz	rain_cnt_l, f	;two byte counter
	goto	int_end
	;WARNING: ignores owerflow, 2 bytes should be enough for about 5 years!
	incf	rain_cnt_h, f
	goto	int_end
;-----------------------------
;restore register values
int_end
	movf	fsr_sav, w
	movwf	fsr

	swapf	stat_sav, w
	movwf	status

	swapf	w_sav, f
	swapf	w_sav, w
;...............................
	retfie
;######################################################################
;Init
;######################################################################
init
;bank 0
	clrf	status
;--------------Oscillator--------------
;bank1
	bsf	status, rp0
	movlw	b'01101110'
	movwf	osccon		;internal 4MHz
;bank0
	bcf	status, rp0
;---------------set ports--------------
;clear ports
	clrf	porta
	clrf	portb
;bank1
	bsf	status, rp0

	clrf	ansel		;all are IO ports

	movlw	0xff
	movwf	trisa
	movwf	trisb
;---------------timers-----------------
;timer 0 - rain sensor
;.....................
	bsf	option_reg, t0cs	;incremented by pulses on T0CKI
;bank 0
	bcf	status, rp0
	clrf	tmr0

;timer 1 - wind speed timebase
;.....................
	movlw	0x09		;internal clock, prescaler 1:1
	movwf	t1con
	clrf	tmr1l
	clrf	tmr1h
;capture
	movlw	0x05
	movwf	ccp1con		;every rising edge

;timer 2 - receiver timing
;.....................
	movlw	0x04		;no prescaler/postscaler
	movwf	t2con
;--------------interrupt----------------
;bank 1
	bsf	status, rp0

	movlw	0x20
	movwf	pie1		;enable UART_RX interrupt

	movlw	0x70
	movwf	intcon		;enable int on portb, 0 and timer 0
;---------------serial-----------------
	movlw	BAUDRATE
	movwf	spbrg		;set baudrate
	movlw	0x24
	movwf	txsta		;high speed, 8 bit, asynchronous mode
;bank 0
	bcf	status, rp0
	movlw	0x90
	movwf	rcsta
;------------constants----------------
	movlw	ERR_TO_FAIL
	movwf	dht22_err
	movwf	bmp180_err
;-------------sensors-----------------
;read bmp180 calibration table
;TODO uncomment
;	call	bmp180_init
	bsf	intcon, gie	;and finally, allow the interrupt
;######################################################################
;main
;######################################################################
start
	call	send_data
loop
	goto	loop

;first, get all the data
	bcf	intcon, gie	;disable interrupts
	call	humidity_temperature_read
	bsf	intcon, gie	;and enable it again

	call	pressure_read
;wind direction
	movlw	0x0f
	andwf	porta, w
	movwf	wind_dir
;copy the data which could be chaged by interrupt
	bcf	intcon, gie
	movf	wind_cnt_l, w
	movwf	wind_speed_l
	movf	wind_cnt_h, w
	movwf	wind_speed_h
	movf	rain_cnt_l, w
	movwf	rain_l
	movf	rain_cnt_h, w
	movwf	rain_h
	bsf	intcon, gie
;send the collected data
	call	send_data
;wait for two seconds
	goto	start
;----------------------------------------------------------------------
	end
