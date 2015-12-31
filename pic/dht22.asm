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
; Read humidity and temperature into into temp_l/h and humidity_l/h
;
; After successful reading, the info, temp_hum_ready is set, if not, this flag
; is not modified for ERR_TO_FAIL times, then is cleared. It allows to ignoring
; momentary transfer errors if the sensor is not dead, just unstable.
;
; Wait at least 2 seconds before another measurement
;
; DISABLE INTERRUPTS before calling
; port dht22 must be set as input with bit in portx set to 0
;
; USES	timer2, fsr
;	stack - one level (wait_ms)
;	count_bytes/bits, checksum, DATA_SPACE_BASE to DATA_SPACE_BASE+4,
;		dht22_err (contains count of erros), info (flags)
;----------------------------
humidity_temperature_read
dht22_read
;send start - low for min 800us
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
	goto	dht22_1		;loop until low
;reply arrived, wait for data
dht22_2
	btfss	dht22
	goto	$-1		;wait for the end of low level on bus - end of response signal

	movlw	5
	movwf	count_bytes	;bytes to read
	movlw	DATA_SPACE_BASE
	movwf	fsr		;base address to write received data to
	clrf	checksum

	btfsc	dht22
	goto	$-1		;wait for beggining of the first bit

dht22_read_data
	clrf	indf
	goto	dht22_byte	;read one byte
dht22_read_data_2
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
;read one byte, assumes bus to be in low
dht22_byte
	movlw	8
	movwf	count_bits
dht22_byte_2
	goto	dht22_bit	;read one bit, result in status, c
dht22_byte_3
	rlf	indf, f		;rotate via carry -> add result from c to register
	decfsz	count_bits, f
	goto	dht22_byte_2	;read another bit
	goto	dht22_read_data_2	;whole byte finished

;read one bit - based on length of high part of pulse
dht22_bit
	btfss	dht22
	goto	$-1		;wait for high
	clrf	tmr2
	bcf	pir1, tmr2if	;prepare timer
dht22_bit_2
	btfsc	pir1, tmr2if
	goto	dht22_error	;error - timeouted
	btfsc	dht22
	goto	dht22_bit_2	;loop until low - start of another bit, end of current
;bit ended, get lenght
	movlw	50
	subwf	tmr2, w		;tmr2 - 50 => set status, c to received bit value
	goto	dht22_byte_3

;...................
;if something failed
dht22_error
	decfsz	dht22_err, f
	return			;not too much errors
	movlw	ERR_TO_FAIL
	movwf	dht22_err
	bcf	info, temp_hum_ready
	return
