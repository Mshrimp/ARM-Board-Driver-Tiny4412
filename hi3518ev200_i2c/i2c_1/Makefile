

KERNEL_DIR = ~/netview/nvc-hi3518ev200-kernel/linux-3.4.y
TARGET_DIR = /mnt/nfs/mcy/hi3518

CROSS_COMPLITE = arm-hisiv300-linux-
CC = $(CROSS_COMPLITE)gcc

DRIVER_NAME = hi3518ev200

DEVICE_RELYON := driver.o
DEVICE_RELYON += hi3518ev200/gpio.o
DEVICE_RELYON += led/led.o
#DEVICE_RELYON += i2c/i2c_gpio.o
DEVICE_RELYON += i2c/i2c.o
#DEVICE_RELYON += fm36/fm36.o

obj-m := $(DRIVER_NAME).o
$(DRIVER_NAME)-objs := $(DEVICE_RELYON)

all:
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(KERNEL_DIR) M=`pwd` clean
	cd test && make clean


app:
	cd test && make

install:
	-cp *.ko $(TARGET_DIR) -rf
	cd test && make install
