
#KERNEL_DIR := /lib/modules/$(shell uname -r)/build
#KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
#KERNEL_DIR := /home/kevin/kernel/linux-3.5
#ROOTFS	:= /home/$(USER)/nfsroot
ROOTFS	:= /nfs_mini
TARGET_DIR := /nfs_mini/mcy

#CROSS_COMPILE=arm-linux-
#CC=$(CROSS_COMPILE)gcc



DRIVER_OBJS += driver/driver.o
DRIVER_LED_NAME := driver_4412_led
obj-m += $(DRIVER_LED_NAME).o
$(DRIVER_LED_NAME)-objs := $(DRIVER_OBJS)

DRIVER_MMA7660_NAME := driver_4412_mma7660
obj-m += $(DRIVER_MMA7660_NAME).o
$(DRIVER_MMA7660_NAME)-objs := $(DRIVER_OBJS)

all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	#arm-linux-gcc app.c -o app
	#-@mkdir target
	#-@mv *.mod.* *.o *.o* *.sym* target -f
	#arm-linux-gcc -Wl,-dynamic-linker=/system/lib/ld-linux.so.3 app.c -o app
	#chmod 775 *.ko
	

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	#@rm target -rf
	@#rm *.ko
	rm app -rf

app:
	arm-linux-gcc app.c -o app

in:
	adb push *.ko /cache
	adb push app /cache
#sudo cp *.ko /nfs_mini/mcy -rf
#sudo cp app /nfs_mini/mcy -rf

install:
	make -C $(KERNEL_DIR) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS) 
	sudo cp app *.ko $(TARGET_DIR) -rf
	


