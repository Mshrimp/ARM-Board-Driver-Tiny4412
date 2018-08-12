




KERNEL_SRC := ~/kernel/linux-3.5

obj-m += i2c-s3c2410.o
obj-m += mma7660.o


all:
	make -C $(KERNEL_SRC) M=`pwd` modules

clean:
	make -C $(KERNEL_SRC) M=`pwd` clean


