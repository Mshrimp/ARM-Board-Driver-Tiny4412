obj-m += adc.o

#KERNEL_DIR := /lib/modules/$(shell uname -r)/build
#KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
#KERNEL_DIR := /home/kevin/kernel/linux-3.5
#ROOTFS	:= /home/$(USER)/nfsroot
ROOTFS	:= /nfs_mini
TARGET_DIR := /nfs_mini/mcy

#CROSS_COMPILE=arm-linux-
#CC=$(CROSS_COMPILE)gcc

vpath %.h	../include


all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	@#mkdir target
	@#mv *.mod.* *.o *.o* *.sym* target -f
	arm-linux-gcc -Wl,-dynamic-linker=/system/lib/ld-linux.so.3 app.c -o app
	chmod 775 adc.ko
#arm-linux-gcc app.c -o app


.PHONY: clean
clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	@#rm target -rf
	@#rm *.ko
	rm app -rf

.PHONY: app
app:
	arm-linux-gcc app.c -o app

.PHONY: in
in:
	adb push adc.ko /cache
	adb push app /cache
#sudo cp *.ko /nfs_mini/mcy -rf
#sudo cp app /nfs_mini/mcy -rf

install:
	make -C $(KERNEL_DIR) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS) 

	


