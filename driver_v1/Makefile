
#KERNEL_DIR := /lib/modules/$(shell uname -r)/build
#KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
#KERNEL_DIR := /home/kevin/kernel/linux-3.5
#ROOTFS	:= /home/$(USER)/nfsroot
ROOTFS	:= /nfs_mini
TARGET_DIR := /nfs_mini/mcy

#CROSS_COMPILE=arm-linux-
#CC=$(CROSS_COMPILE)gcc


obj-m += led/tiny4412_led.o

all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	@cp led/*.ko build/ -rf

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	@rm build/* -rf

app:
	arm-linux-gcc demo/tiny4412_led.c -o build/tiny4412_led 

in:
	adb push *.ko /cache
	adb push app /cache
#sudo cp *.ko /nfs_mini/mcy -rf
#sudo cp app /nfs_mini/mcy -rf

install:
	make -C $(KERNEL_DIR) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS) 
	sudo cp app *.ko $(TARGET_DIR) -rf
	


