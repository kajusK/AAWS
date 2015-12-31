;**********************************************************************
; Weather station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; bmp180.asm
;------------------------------
; BMP180 air pressure and temperature module
; http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
;
; Requires I2C
;**********************************************************************
;----------------------------
; read calibration data from bmp180 eprom
; calibration data are stored from BMP_CAL_BASE to the base + BMP_COEF_NUM -1
;
; returns 1 in status,c if succeeded, or 0
;
; USES	fsr
;	stack - 2 levels (bmp180_read/write)
;	data1, BMP_CAL_BASE - (BMP_CAL_BASE + BMP_COEF_NUM - 1)
;---------------------------
bmp180_init
;send address to read from
	movlw	data1
	movwf	fsr
	movlw	BMP_COEF_BASE	;addres in remote memory to read from
	movwf	data1
	movlw	1
	call	bmp180_write
	btfss	status, c
	return
;read all calibration data from eprom
	movlw	BMP_CAL_BASE
	movwf	fsr		;store data at this addres
	movlw	BMP_COEF_NUM
	call	bmp180_read
	btfss	status, c
	return
	bsf	info, bmp180_set	;set flag that data are ready
	return

;----------------------------
; Read temperature and pressure from sensor
;
; data will be stored in pressure_h/l
;
; After successful reading, the info, pressure_ready is set, if not, this flag
; is not modified for ERR_TO_FAIL times, then is cleared. It allows ignoring
; momentary transfer errors if the sensor is not dead, just unstable.
;
; DISABLES INTERRUPT few times
;
; USES	fsr
;	stack - 3 levels (bmp180_cmd/read_data,wait_ms,)
;	DATA_SPACE_BASE - DATA_SPACE_BASE+4, bmp180_err (err count), info (flags)
;----------------------------
pressure_read
;first, we need to get the temperature
	movlw	BMP_TEMP_CMD
	call	bmp180_cmd	;measure temperature
	btfss	status, c
	goto	pressure_error	;failed to communicate

	movlw	5
	call	wait_ms		;wait for longet than 4,5ms for result

	call	bmp180_read_data ;read the temperature
	btfss	status, c
	goto	pressure_error	;communication failed

;TODO: do the math with the temperature

;get the pressure
	movlw	BMP_PRESR_CMD
	call	bmp180_cmd	;measure air pressure

	btfss	status, c
	goto	pressure_error	;failed to communicate

	movlw	14
	call	wait_ms		;wait for longet than 13,5ms for result

	call	bmp180_read_data ;read the pressure
	btfss	status, c
	goto	pressure_error	;communication failed

;TODO: do the math magic
	;check info, bmp180_set if there are calibration data!
	clrf	bmp180_err
	bsf	info, pressure_ready
	return

;..........
pressure_error
	decfsz	bmp180_err, f
	return			;not too much errors
	movlw	ERR_TO_FAIL
	movwf	bmp180_err
	bcf	info, pressure_ready
	return


;----------------------------
; write data to device
;
; w contains amount of bytes to write
; data are taken from fsr addressed byte and w - 1 following ones
; returns 1 in status,c if succeeded, or 0
;
; USES	fsr
;	stack - 1 level (i2c_start/stop/tx))
;	count_bytes
;----------------------------
bmp180_write
	movwf	count_bytes	;save bytes to send
	call	i2c_start
	movlw	BMP_WRITE_ADDR
	call	i2c_tx		;send device address
	btfss	status, c
	goto	i2c_helper	;ack didn't arrived, return false

bmp180_write_2
	movf	indf, w
	call	i2c_tx
	btfss	status, c
	goto	i2c_helper	;ack did not arrived, return false

	incf	fsr, f
	decfsz	count_bytes, f
	goto	bmp180_write_2
	bsf	status, c	;return value
	goto	i2c_helper	;suceeded, status, c is 1

;----------------------------
; read data from device
; w contains amount of bytes to read
; data are stored on fsr addressed byte and following ones
; returns status, c = 1 if suceeded, 0 otherwise
; USES	fsr
;	stack - 1 level (i2c_start/stop/tx/rx))
;	count_bytes
;----------------------------
bmp180_read
	movwf	count_bytes
	call	i2c_start

	movlw	BMP_READ_ADDR
	call	i2c_tx		;send device address
	btfss	status, c
	goto	i2c_helper	;no reply, return error

	bsf	info, ack	;we will send ack on the end of each byte
bmp180_read_2
	call	i2c_rx
	movwf	indf		;save received data
	incf	fsr, f		;move to the next address

	decfsz	count_bytes, f
	goto	bmp180_read_2
	bsf	status, c	;return value
	goto	i2c_helper	;all get, return

;----------------------------
; Send command
; w contains command
; returns 1 in status,c if succeeded, or 0
; USES	fsr
;	stack - 2 level (bmp180_write)
;	data1/2
;----------------------------
bmp180_cmd
	movwf	data2		;command
	movlw	data1
	movwf	fsr		;addres to get data from
	movlw	BMP_CONT_REG	;addres in remote memory to write to
	movwf	data1
	movlw	2
	bcf	intcon, gie	;disable interrupts
	call	bmp180_write	;and send
	bsf	intcon, gie
	return

;----------------------------
; Read lsm and msb
; data are stored in data1 (msb) and data2(lsb)
; returns 1 in status,c if succeeded, or 0
; USES	fsr
;	stack - 2 level (bmp180_write/read)
;	data1/2
;----------------------------
bmp180_read_data
;send address to read from
	movlw	data1
	movwf	fsr
	movlw	BMP_DATA_BASE	;addres in remote memory to read from
	movwf	data1
	movlw	1
	bcf	intcon, gie
	call	bmp180_write
	bsf	intcon, gie
	btfss	status, c
	return
;read msb and lsb to data 1/2
	movlw	data1
	movwf	fsr
	movlw	2
	bcf	intcon, gie
	call	bmp180_read
	bsf	intcon, gie
	return
