
DRV_NAME := driver
obj-m += $(DRV_NAME).o

USER_DIR := /home/$(USER)

KERNEL_DIR := $(USER_DIR)/kernel/linux-3.5
ROOTFS	:= /nfs_mini
TARGET_DIR := $(ROOTFS)/mcy


all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	-@#arm-linux-gcc app.c -o app
	-@#arm-linux-gcc -Wl,-dynamic-linker=/system/lib/ld-linux.so.3 app.c -o app
	chmod 775 *.ko

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	-rm app -rf

app:
	arm-linux-gcc app.c -o app

in:
	adb push *.ko /cache
	adb push app /cache

install:
	-@#make -C $(KERNEL_DIR) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS) 
	-sudo cp *.ko $(TARGET_DIR) -rf
	-sudo cp app $(TARGET_DIR) -rf
	


