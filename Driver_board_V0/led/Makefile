
name=led
bin=$(name).bin
obj=start.o
obj+=$(name).o
tar=$(name).c


#$(name).bin : $(obj)
all:$(obj)
	arm-none-linux-gnueabi-gcc -c $^ -o $@
	arm-none-linux-gnueabi-ld -T $(name).lds -N $(name).o -o $(name).elf
	arm-none-linux-gnueabi-objcopy -O binary -S $(name).elf $(name).bin
	arm-none-linux-gnueabi-objdump -D -m arm $(name).elf > $(name).dis

%.o:%.c
	arm-none-linux-gnueabi-gcc -Wall -marm -c -O2 -o $@ $<

%.o:%.S
	arm-none-linux-gnueabi-gcc -Wall -marm -c -O2 -o $@ $<

clean:
	rm -f $(bin) $(obj) $(name) *.elf *.o *.bin *.dis


