;**********************************************************************
; Weather station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; uart.asm
;------------------------------
; Basic serial communication routines + text sending from tables
;**********************************************************************
;-----------------------------
; receive on byte to W
;
; will loop until there's a character in input buffer
;-----------------------------
uart_rx
	btfss	rcsta, oerr
	goto	uart_rx_1
;an overrun occured, reset receiver
	bcf	rcsta, cren
	bsf	rcsta, cren
uart_rx_1
	btfss	pir1, rcif
	goto	uart_rx_1
	movf	rcreg, w
	return

;-----------------------------
;send byte from W
;-----------------------------
uart_tx
	btfss	pir1, txif
	goto	uart_tx
	movwf	txreg
	return

;----------------------------
; send the data collected
; USES	fsr
;	stack - 3 level (send_string/digits/error, uart_tx, bin2bcd)
;	table_offset, char_count
;----------------------------
send_data
	bcf	info, decimal	;used for printing decimal place where needed
;temperature
	movlw	T_TEMP_START
	movwf	table_offset
	movlw	T_TEMP_LENGTH
	call	send_string	;send Temp:

	btfss	info, temp_hum_ready
	goto	send_data_temp_error	;no data ready, sensor could be dead

	movlw	temp_h
	movwf	fsr
	bsf	info, decimal	;temp has one decimal space

	btfsc	temp_h, 7
	goto	send_data_temp_signed

	call	bin2bcd	;get the digits
	call	send_digits	;print the digits
	goto	send_data_temp2

send_data_temp_signed
	movlw	'-'
	call	uart_tx
	bcf	temp_h, 7	;temporary, don't forget to set it back
	call	bin2bcd
	call	send_digits	;show the number
	bsf	temp_h, 7	;set the sign back
send_data_temp2
	movlw	T_TEMP_C_LENGTH
	call	send_string	;table_offset is at the right spot from before

;humidity
send_data_hum
	movlw	T_HUM_START
	movwf	table_offset
	movlw	T_HUM_LENGHT
	call	send_string	;send Hum:

	btfss	info, temp_hum_ready
	goto	send_data_hum_error

	movlw	humidity_h
	movwf	fsr
	call	bin2bcd
	call	send_digits

	movlw	T_RH_LENGTH
	call	send_string	;offset is at the right spot from before
;pressure
send_data_press
	bcf	info, decimal	;it's not needed anymore

	movlw	T_PRESS_START
	movwf	table_offset
	movlw	T_PRESS_LENGTH
	call	send_string	;send Pres:

	btfss	info, pressure_ready
	goto	send_data_pres_error

	movlw	pressure_h
	movwf	fsr
	call	bin2bcd
	call	send_digits

	movlw	T_PA_LENGTH
	call	send_string	;offset it at the right stop from before

;wind informations
send_data_wind
	movlw	T_WIND_START
	movwf	table_offset
	movlw	T_WIND_LENGTH
	call	send_string	;send Wind\n
;wind speed
	movlw	T_SPEED_LENGTH
	call	send_string	;send speed:

	bsf	info, decimal	;will send one decimal space
	movlw	wind_speed_h
	movwf	fsr
	call	bin2bcd
	call	send_digits	;send speed
	bcf	info, decimal

	movlw	T_MS_LENGTH
	call	send_string	;send m/s\n

;wind_direction
	movlw	T_DIR_LENGTH
	call	send_string	;send wind direction

	movf	wind_dir, w
	call	table_wind_dir
	movwf	wind_dir_l
	clrf	wind_dir_h

	movlw	12
	subwf	wind_dir, w
	btfsc	status, c
	incf	wind_dir_h,f	;direction is more than 255°
	movlw	wind_dir_h
	movwf	fsr
	call	bin2bcd
	call	send_digits	;show the number

	movlw	T_DEG_LENGTH
	call	send_string	;send *\n
;rain
	movlw	T_RAIN_LENGTH
	call	send_string	;send Rain:

	bsf	info, decimal	;will send one decimal space
	movlw	rain_h
	movwf	fsr
	call	bin2bcd
	call	send_digits
	bcf	info, decimal

	movlw	T_MM_LENGTH
	call	send_string	;send mm
;done, send new line and return
	movlw	'\n'
	call	uart_tx
	return			;all done
;..............
send_data_temp_error
	call	send_error
	goto	send_data_hum
send_data_hum_error
	call	send_error
	goto	send_data_press
send_data_pres_error
	call	send_error
	goto	send_data_wind
;----------------------------
; send string from table_string
; the offset to table is in table_offset
; the length of data to send is in w
; USES	stack - 1 level (uart_tx)
;	table_offset, char_count
;----------------------------
send_string
	movwf	char_count
send_string_1
	movf	table_offset, w
	call	table_string	;get the character
	call	uart_tx		;and send it
	incf	table_offset, f
	decfsz	char_count, f
	goto	send_string_1
	return
;----------------------------
; send digits from data1 (tens) - data5 (ten thousands)
; suppresses leading zeros, if info, decimal is set, places , before last digit
; appends space at the end
; USES	fsr
;	stack - 1 level (uart_tx)
;	char_count
;----------------------------
send_digits
	movlw	5-1		;number of digits - 1
	btfsc	info, decimal
	movlw	5-2		;we need to take care of the digit before zero
	movwf	char_count
	movlw	data5
	movwf	fsr
;skip leading zeros
send_digits2
	movf	indf, f
	btfss	status, z
	goto	send_digits3	;digit isn't zero, ready to print
	decf	fsr, f
	decfsz	char_count, f
	goto	send_digits2
;now, there should be only digits to print
send_digits3
	incf	char_count, f	;now, we want to print last (or last-1) digit
send_digits4
	movf	indf, w
	addlw	'0'		;convert to ascii
	call	uart_tx
	decf	fsr, f
	decfsz	char_count, f
	goto	send_digits4

	btfss	info, decimal
	goto	send_digits_end	;all send, finish

;send the decimal and last place
	movlw	'.'
	call	uart_tx

	movf	indf, w
	addlw	'0'
	call	uart_tx

send_digits_end
	movlw	' '
	call	uart_tx
	return
;----------------------------
; send ERROR\n
; USES	stack - 2 level (send_string)
;----------------------------
send_error
	movlw	T_ERROR_START
	movwf	table_offset
	movlw	T_ERROR_LENGTH
	call	send_string
	return

