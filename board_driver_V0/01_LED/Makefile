
CROSS_COMPILE = arm-linux-

CC = $(CROSS_COMPILE)gcc
CC_FLAGS = -c -I .

LD = $(CROSS_COMPILE)ld
LD_FLAGS = -T start.lds

OBJCOPY = $(CROSS_COMPILE)objcopy
OBJCOPY_FLAGS = -O binary

OBJDUMP = $(CROSS_COMPILE)objdump
OBJDUMP_FLAGS = -D

TARGET = start

OBJS = $(TARGET).o main.o led.o

all: $(OBJS)
	$(LD) $(LD_FLAGS) $(OBJS) -o $(TARGET)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(TARGET) $(TARGET).bin
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(TARGET) > $(TARGET).dis
	@mkdir target
	@mv *.o *.dis *.bin $(TARGET) target
	@echo -e '\033[1;32m ========== Target OK ========== \033[0m'

%.o:%.s
	$(CC) $(CC_FLAGS) $^
%.o:%.S
	$(CC) $(CC_FLAGS) $^
%.o:%.c
	$(CC) $(CC_FLAGS) $^

clean:
	@rm -rf *.o *.dis $(TARGET) *.bin *.elf
	@rm target -rf
	@echo -e '\033[1;32m ========= Target clean ======== \033[0m'
#@echo -e "\e[1;31m======== Target clean ========\e[0m"





