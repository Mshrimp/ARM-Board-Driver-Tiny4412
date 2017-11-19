obj-m += globalmem.o

KERNEL_DIR := /usr/src/linux-headers-4.4.0-97-generic



all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	sudo insmod globalmem.ko

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean 
	sudo rmmod globalmem


