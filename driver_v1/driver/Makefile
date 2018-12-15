
KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
ROOTFS_DIR	:= /nfs_mini
TARGET_DIR := $(ROOTFS_DIR)/mcy
PWD := $(shell pwd)

DRV_NAME := driver
obj-m := $(DRV_NAME).o
$(DRV_NAME)-objs := driver.c

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	#make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	@rm app -rf

app:
	arm-linux-gcc app.c -o app

install:
	make -C $(KERNEL_DIR) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS_DIR) 
	sudo cp app *.ko $(TARGET_DIR) -rf

