all:
	arm-linux-gcc -c -o uart.o uart.c
	arm-linux-gcc -c -o main.o main.c
	arm-linux-gcc -c -o led.o led.c
	arm-linux-gcc -c -o init.o init.c
	arm-linux-gcc -c -o start.o s3c2400_start.S
	arm-linux-ld -T sdram.lds start.o init.o led.o uart.o main.o -o relocate.elf
	arm-linux-objcopy -O binary -S relocate.elf relocate.bin
	arm-linux-objdump -D relocate.elf > relocate.dis
clean:
	rm *.bin *.o *.elf *.dis