NAME=weather
MCU=PIC16F88

${NAME}.hex: *.asm
	gpasm -i ${NAME}.asm

burn:
	pk2cmd -P${MCU} -F ${NAME}.hex -M -JN -W

verify:
	pk2cmd -P${MCU} -F ${NAME}.hex -Y

clean:
	rm *.hex *.lst *.cod

