KERN_DIR = /tools/system/linux_2_6_226/linux-2.6.22.6

all:
	make -C $(KERN_DIR) M=`pwd` modules

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= poll.o