;**********************************************************************
; Weather station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; dht22.asm
;------------------------------
; DHT22 (or AM2302 and similar) humidity and temperature sensor
; http://akizukidenshi.com/download/ds/aosong/AM2302.pdf
;**********************************************************************
;----------------------------
; DHT22 read humidity and temperature - wait at least 2 seconds between readings
; data will be stored on temp_l/h, humidity_l/h
; if failed, the info, temp_hum_ready will be 0
; port dht22 must be set as input with bit in portx set to 0
; DISABLE INTERRUPTS before calling
; USES	timer2, fsr
;	stack - one level (wait_ms)
;	count_bytes/bits, checksum, DATA_SPACE_BASE - DATA_SPACE_BASE+4,
;		dht22_err, info
;----------------------------
humidity_temperature_read
dht22_read
;send start
	bsf	status, rp0
	bcf	dht22
	movlw	1
	call	wait_ms
	bsf	dht22
	bcf	status, rp0
;wait for reply
	clrf	tmr2
	bcf	pir1, tmr2if
dht22_1
	btfsc	pir1, tmr2if
	goto	dht22_error	;error, no reply within 255 us

	btfsc	dht22
	goto	dht22_1
;reply arrived, wait for data
dht22_2
	btfss	dht22
	goto	dht22_2		;wait for the end of low level on bus

;read 5 bytes
	movlw	5
	movwf	count_bytes
	movlw	DATA_SPACE_BASE
	movwf	fsr
	clrf	checksum
dht22_read_data
	clrf	indf
	goto	dht22_byte	;read one byte
	movf	indf, w
	addwf	checksum, f	;add received data to checksum
	incf	fsr, f		;move to next byte

	decfsz	count_bytes, f
	goto	dht22_read_data
;verify crc
	decf	fsr, f
	movf	indf, w
	subwf	checksum, f	;it contains also the checksum itself, delete it
	subwf	checksum, w	;now, we can werify if it is correct
	btfss	status, z
	goto	dht22_error	;checksum not correct
;all data correct
	clrf	dht22_err	;clear latest errors
	movf	data1, f
	movwf	humidity_h
	movf	data2, f
	movwf	humidity_l
	movf	data3, f
	movwf	temp_h
	movf	data3, f
	movwf	temp_l		;move all the data to the newes location
	bsf	info, temp_hum_ready
	return			;finished
;..........................
;read one byte
dht22_byte
	movlw	8
	movwf	count_bits
dht22_byte2
	goto	dht22_bit
	rlf	indf, f		;rotate via carry, the result is in carry
	decfsz	count_bits, f
	goto	dht22_byte2
	goto	dht22_read_data

;read one bit
dht22_bit
	btfss	dht22
	goto	dht22_bit
	clrf	tmr2		;high, wait for down
	bcf	pir1, tmr2if
dht22_bit2
	btfsc	pir1, tmr2if
	goto	dht22_error	;error - timeouted
	btfsc	dht22
	goto	dht22_bit2
;bit ended, get lenght
	movf	tmr2, w
	sublw	50		;result is in status, c, if c==1, bit is 1
	goto	dht22_byte2

;...................
;if something failed
dht22_error
	decfsz	dht22_err, f
	return			;not too much errors
	movlw	ERR_TO_FAIL
	movwf	dht22_err
	bcf	info, temp_hum_ready
	return
