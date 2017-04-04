
obj-m += led.o

KERNEL_PATH := /home/kevin/kernel/linux-3.5
ROOTFS := /nfsroot

all:
	make -C $(KERNEL_PATH) M=`pwd` modules


app:
	arm-linux-gcc app.c -o app


clean:
	make -C $(KERNEL_PATH) M=`pwd` clean

install:
	make -C $(KERNEL_PATH) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS)


