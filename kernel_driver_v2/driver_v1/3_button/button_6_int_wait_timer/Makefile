obj-m += key.o

KERNEL_DIR := /home/$(USER)/kernel/linux-3.5
ROOTFS	:= /nfs_mini
TARGET_DIR := $(ROOTFS)/mcy

all:
	make -C $(KERNEL_DIR) M=`pwd` modules
	arm-linux-gcc app.c -o app
	@#-@mkdir target
	@#-@mv *.mod.* *.o *.o* *.sym* target -f
	@#arm-linux-gcc -Wl,-dynamic-linker=/system/lib/ld-linux.so.3 app.c -o app
	chmod 775 *.ko

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	@#rm target -rf
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
	@#make -C $(KERNEL_DIR) M=`pwd` modules_install INSTALL_MOD_PATH=$(ROOTFS) 
	sudo cp app *.ko $(TARGET_DIR) -rf

