
ifeq ($(KERNELRELEASE),)
	KERNEL_DIR = /home/kevin/kernel/linux-3.5
	PWD = $(shell pwd)

modules:
	echo "make modules"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

app:
	arm-linux-gcc beep_test.c -o test

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm test -rf

else
	obj-m := beep.o

endif
	
