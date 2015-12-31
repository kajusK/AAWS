;**********************************************************************
; Weather Station
; PIC 16F88
; internal 4MHz
;------------------------------
; Jakub Kaderka
; jakub.kaderka@gmail.com
; 2015
;------------------------------
; tables.asm
;------------------------------
; Call with offset to table in W
; the result will be returned in W
;**********************************************************************
table_string
	addwf pcl, f
;-------------Temp: -----------
T_TEMP_START	equ	0
T_TEMP_LENGTH	equ	6
	retlw 'T'
	retlw 'e'
	retlw 'm'
	retlw 'p'
	retlw ':'
	retlw ' '

;-------------*C\n-------------
;must be right behind the temp
;° is not unicode character, therefore using *
T_TEMP_C_START	equ	6
T_TEMP_C_LENGTH	equ	3
	retlw '*'
	retlw 'C'
	retlw '\n'

;-------------Hum: ------------
T_HUM_START	equ	9
T_HUM_LENGHT	equ	5
        retlw 'H'
        retlw 'u'
        retlw 'm'
	retlw ':'
	retlw ' '

;------------%RH\n-------------
;must be right behind the hum
T_RH_START	equ	14
T_RH_LENGTH	equ	4
        retlw '%'
        retlw 'R'
        retlw 'H'
        retlw '\n'
;----------Press: -------------
T_PRESS_START	equ	18
T_PRESS_LENGTH	equ	6
        retlw 'P'
        retlw 'r'
        retlw 'e'
        retlw 's'
	retlw ':'
	retlw ' '
;-----------Pa\n---------------
;must be right behind the pres
T_PA_START	equ	24
T_PA_LENGTH	equ	3
        retlw 'P'
        retlw 'a'
        retlw '\n'
;----------Wind\n------------
T_WIND_START equ 27
T_WIND_LENGTH equ 5
        retlw 'W'
        retlw 'i'
        retlw 'n'
        retlw 'd'
        retlw '\n'
;----------\tSpeed: ----------
;must be right behind the wind
T_SPEED_START equ 32
T_SPEED_LENGTH equ 8
        retlw '\t'
        retlw 'S'
        retlw 'p'
        retlw 'e'
        retlw 'e'
        retlw 'd'
        retlw ':'
        retlw ' '
;----------m/s/n------------
;must be right behind the speed
T_MS_START equ 40
T_MS_LENGTH equ 4
        retlw 'm'
        retlw '/'
        retlw 's'
        retlw '\n'
;----------\tDir: ------------
;must be right behind the m/s
T_DIR_START equ 44
T_DIR_LENGTH equ 6
	retlw '\t'
        retlw 'D'
        retlw 'i'
        retlw 'r'
        retlw ':'
        retlw ' '
;----------*\n------------
;must be right behind the dir
T_DEG_START equ 50
T_DEG_LENGTH equ 2
        retlw '*'
        retlw '\n'
;----------Rain:------------
;must be right behind the *
T_RAIN_START equ 52
T_RAIN_LENGTH equ 6
        retlw 'R'
        retlw 'a'
        retlw 'i'
        retlw 'n'
        retlw ':'
	retlw ' '
;----------mm\n------------
;must be right behind the mm
T_MM_START equ 58
T_MM_LENGTH equ 3
        retlw 'm'
        retlw 'm'
        retlw '\n'
;-----------ERROR\n------------
T_ERROR_START	equ	61
T_ERROR_LENGTH	equ	6
        retlw 'E'
        retlw 'R'
        retlw 'R'
        retlw 'O'
        retlw 'R'
        retlw '\n'

;-----------------------------
; wind direction
;-----------------------------
table_wind_dir
	addwf pcl, f
	retlw	0
	retlw	22
	retlw	45
	retlw	67
	retlw	90
	retlw	112
	retlw	135
	retlw	157
	retlw	180
	retlw	202
	retlw	225
	retlw	247
;from here, the numbers are calculated to fit 0x01xx
	retlw	0x0e
	retlw	0x24
	retlw	0x3b
	retlw	0x51
