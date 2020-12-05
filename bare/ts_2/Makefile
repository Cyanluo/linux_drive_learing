objs = s3c2400_start.o led.o uart.o init.o main.o exception.o my_printf.o string_utils.o lib1funcs.o

all: $(objs)
	arm-linux-ld -T sdram.lds $^ -o swi.elf
	arm-linux-objcopy -O binary -S swi.elf swi.bin
	arm-linux-objdump -D swi.elf > swi.dis

clean:
	rm *.o *.bin *.dis *.elf

%.o: %.c
	arm-linux-gcc -c -o $@ $<

%.o: %.S
	arm-linux-gcc -c -o $@ $<