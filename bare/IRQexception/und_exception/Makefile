all: s3c2400_start.o led.o uart.o init.o main.o exception.o
	arm-linux-ld -T sdram.lds $^ -o exception1.elf
	arm-linux-objcopy -O binary -S exception1.elf exception.bin
	arm-linux-objdump -D exception1.elf > exception1.dis

clean:
	rm *.o *.bin *.dis *.elf

%.o: %.c
	arm-linux-gcc -c -o $@ $<

%.o: %.S
	arm-linux-gcc -c -o $@ $<