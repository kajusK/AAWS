;**********************************************************************
; Weather station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; math.asm
;------------------------------
; Various mathematical functions
;**********************************************************************
;----------------------------
; Convert 16bit unsigned number to bcd
; Fsr points to msb, the lsb is the next address, input won't be modified
; the result is stored in data1(ones)-data5(ten thousands)
; USES  fsr
;	data1-data5
;
;
; For sane ones, here's the algorithm:
; (from http://www.piclist.com/techref/microchip/math/radix/b2bu-16b5d.htm)
;
; The input can be written as a3*16^3 + a2*16^2 + a1*a1^1 + a0*16^0
; Which is stored as msb = a3*16+a2 and lsb=a1*16+a0
; we want to get b4*10^4 + b3*10^3  + b2*10^2 + b1*10^1 + b0*10^0
; the number can be written as N = a_3*4096 + a_2*256 + a_1*16 + a_0
; or as N=a_3*(4100 - 4) + a_2*(260 - 4) + a_1*(20-4) + a_0 =
; = 4*a_3*10^3 + (a_3 + 2*a_2)*10^2 + (6*a_2 + 2*a_1)*10 + a_0 - 4*(a_3 + a_2 + a_1)
; this equation gives:
;
; b_0 = a_0 - 4*(a_3 + a_2 + a_1)
; b_1 = 6*a_2 + 2*a_1
; b_2 = a_3 + 2*a_2
; b_3 = 4*a_3
; b_4 = 0
;
; And now some magic:
; as the calculated numbers could be out of range 0-9, we need to do some more
; calculations. Since the a_n must be in 0-15, the b_0 could be from -60 to 15
; so, we can write b1*10 + b0 = b_1*10 + b0 + 20 - 20 = (b1+2)*10+b0-20
; do something similar for the rest and you get:
; b_0 = a_0 - 4*(a_3 + a_2 + a_1) - 20
; b_1 = 6*a_2 + 2*a_1 + 2 - 140 = 6*a_2 + 2*a_1 - 138
; b_2 = a_3 + 2*a_2 + 14 - 60 = a_3 + 2*a_2 - 46
; b_3 = 4*a_3 + 6 - 70 = 4*a_3 - 64
; b_4 = 0 + 7 = 7
; only problem left is converting it to ranges 0-9
;----------------------------
bin2bcd
	swapf	indf, w	;w  = A2*16+A3 (swapped msb)
	iorlw	0xf0	;w  = A3-16 (in two's complement)
	movwf	data4	;B3 = A3-16
	addwf	data4,f ;B3 = 2*(A3-16) = 2A3 - 32
	addlw	226	;w  = A3-16 - 30 = A3-46
	movwf	data3	;B2 = A3-46
	addlw	50	;w  = A3-46 + 50 = A3+4
	movwf	data1	;B0 = A3+4

	movf	indf,w	;w  = A3*16+A2 (msb)
	andlw	0x0F	;w  = A2
	addwf	data3,f	;B2 = A3-46 + A2 = A3+A2-46
	addwf	data3,f	;B2 = A3+A2-46  + A2 = A3+2A2-46
	addwf	data1,f	;B0 = A3+4 + A2 = A3+A2+4
	addlw	233	;w  = A2 - 23
	movwf	data2	;B1 = A2-23
	addwf	data2,f	;B1 = 2*(A2-23)
	addwf	data2,f	;B1 = 3*(A2-23) = 3A2-69

	incf	fsr, f	;move to lsb

	swapf	indf,w	;w  = A0*16+A1 (swapped lsb)
	andlw	0x0F	;w  = A1
	addwf	data2,f	;B1 = 3A2-69 + A1 = 3A2+A1-69 range -69...-9
	addwf	data1,f	;B0 = A3+A2+4 + A1 = A3+A2+A1+4, carry = 0

	rlf	data2,f	;B1 = 2*(3A2+A1-69) + C = 6A2+2A1-138, Carry is now 1 as tens had to be negitive
	rlf	data1,f	;B0 = 2*(A3+A2+A1+4) + C = 2A3+2A2+2A1+9
	comf	data1,f	;B0 = ~(2A3+2A2+2A1+9) = -2A3-2A2-2A1-10 (ones complement plus 1 is twos complement)

	rlf	data1,f	;B0 = 2*(-2A3-2A2-2A1-10) = -4A3-4A2-4A1-20

	movf	indf,w	;w  = A1*16+A0 (lsb)
	andlw	0x0F	;w  = A0
	addwf	data1,f	;B0 = -4A3-4A2-4A1-20 + A0 = A0-4(A3+A2+A1)-20 range -215...-5 Carry=0
	rlf	data4,f	;B3 = 2*(2A3 - 32) = 4A3 - 64

	movlw	0x07	;w  = 7
	movwf	data5	;B4 = 7
;B0 = A0-4(A3+A2+A1)-20	;-5...-200
;B1 = 6A2+2A1-138	;-18...-138
;B2 = A3+2A2-46		;-1...-46
;B3 = 4A3-64		;-4...-64
;B4 = 7			;7

	movlw	10	;w  = 10
bin2bcd1:
	addwf	data1,f		; B0 += 10
	decf	data2,f		; B1 -= 1
	btfss	status,c
	goto	bin2bcd1	; while B0 < 0
bin2bcd2:
	addwf	data2,f		; B1 += 10
	decf	data3,f		; B2 -= 1
	btfss	status, c
	goto	bin2bcd2	; while B1 < 0
bin2bcd3:			;do
	addwf	data3,f		; B2 += 10
	decf	data4,f		; B3 -= 1
	btfss	status, c
	goto	bin2bcd3	; while B2 < 0
bin2bcd4:
	addwf	data4,f		; B3 += 10
	decf	data5,f		; B4 -= 1
	btfss	status, c
	goto	bin2bcd4	; while B3 < 0

	return
