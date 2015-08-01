;**********************************************************************
; Weather station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; i2c.asm
;**********************************************************************
;**********************************************************************
; I2C
; ports must be set to inputs with values 0 in portx
; disable interrupts before calling
;**********************************************************************
;-----------------------------
; start byte
;-----------------------------
i2c_start
	bsf	status,rp0
	bcf	sda
	nop
	nop
	nop
	bcf	scl
	bcf	status, rp0
;-----------------------------
; stop byte
;-----------------------------
i2c_stop
	bsf	status, rp0
	bsf	scl
	bcf	status, rp0
i2c_stop_2
	btfss	scl
	goto	i2c_stop_2	;wait for slave to release the clock
	bsf	status, rp0
	bsf	sda
	bcf	status, rp0
;----------------------------
; send byte from W
; expects scl to be pulled to low
; ack is in status, c; 1 if arrived
; USES i2c_data, count_bits, info
;----------------------------
i2c_tx
	bsf	status, rp0
	movwf	i2c_data
	movlw	8
	movwf	count_bits
i2c_tx1
	rlf	i2c_data, f	;move bit to send to status, c
	bcf	sda
	btfsc	status, c
	bsf	sda		;set sda to the status, c value
	bsf	scl
	bcf	status, rp0
i2c_tx2
	btfss	scl
	goto	i2c_tx2		;wait for slave to release the clock
	bsf	status, rp0
	bcf	scl
	decfsz	count_bits, f
	goto	i2c_tx1		;send another bit
;.........................
	nop
	bsf	scl
	bcf	status, rp0
i2c_tx3
	btfss	scl
	goto	i2c_tx3		;wait for slave to release the clock
;get ack
	bsf	status, c
	btfsc	sda
	bcf	status, c

	bsf	status, rp0
	bcf	scl
	bcf	status, rp0	;scl back to 0
	return
;----------------------------
; read byte to W
; expects scl to be pulled to low
; ack is in info, ack
; USES i2c_data, count_bits, info
;----------------------------
i2c_rx
	bsf	status, rp0
	bsf	sda
	movlw	8
	movwf	count_bits
	clrf	i2c_data
i2c_rx1
	bsf	scl
	bcf	status, rp0
i2c_rx2
	btfss	scl
	goto	i2c_rx2		;wait for slave to release the clock
	bsf	status, c
	btfss	sda
	bcf	status, c

	bsf	status, rp0
	rlf	i2c_data, f	;insert received bit
	bcf	scl		;clock pulse
	decfsz	count_bits, f
	goto	i2c_rx1		;send another bit
;............................
	btfsc	info, ack
	bcf	sda		;send ack
	bsf	scl
	bcf	status, rp0
i2c_rx3
	btfss	scl
	goto	i2c_rx3
	bsf	status, rp0
	bcf	scl
	movlw	i2c_data
	bcf	status, rp0
	return
;----------------------------
; helper to call i2c_stop and return
; useful to goto here at the btfss/c when needed to exit the function
; will not chage status bits
;----------------------------
i2c_helper
	call	i2c_stop
	return

