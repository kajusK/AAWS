;------------------------------
;Weather station with UART output
;routines.asm
;internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2014
;------------------------------

;**********************************************************************
; wait - valid for 4MHz
; USES wait1, wait2
;**********************************************************************
;-----------------------------
;wait for W*4+3 cycles
;-----------------------------
wait_c
	movwf	wait1
wait_c1
	nop
	decfsz	wait1, f
	goto	wait_c1
	return
;-----------------------------
;wait for W milliseconds + 1 or 3 cycles
; USES
;	stack - one level (wait_c)
;-----------------------------
wait_ms
	movwf	wait2
wait_ms1
	movlw	247
	call	wait_c
	nop
	nop
	nop
	decfsz	wait2, f
	goto	wait_ms1
	return


