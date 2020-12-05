objs = s3c2400_start.o init.o uart.o led.o  main.o exception.o irq.o eint.o 

objs += touch_lcd.o
objs += touchscreentest.o
objs += timer.o

all: $(objs)
	arm-linux-ld -T sdram.lds $^  -o eint.elf
	arm-linux-objcopy -O binary -S eint.elf eint.bin
	arm-linux-objdump -D eint.elf > eint.dis

clean:
	rm -f *.bin $(objs) *.elf *.dis

%.o : %.c
	arm-linux-gcc -march=armv4 -c -o $@ $<

%.o : %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
