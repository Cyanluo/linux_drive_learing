all: s3c2400_start.o led.o uart.o init.o main.o exception.o irq.o
	arm-linux-ld -T sdram.lds $^ -o eint.elf
	arm-linux-objcopy -O binary -S eint.elf eint.bin
	arm-linux-objdump -D eint.elf > eint.dis

clean:
	rm *.o *.bin *.dis *.elf

%.o: %.c
	arm-linux-gcc -c -o $@ $<

%.o: %.S
	arm-linux-gcc -c -o $@ $<