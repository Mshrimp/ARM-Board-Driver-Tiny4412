
CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
CC_FLAGS = -c
CC_FLAGS += -I .
LD = $(CROSS_COMPILE)ld
#-Ttext 0x50000000 -Tdata 0x50000400 指定代码段的起始地址 
LD_FLAGS = -T start.lds
LD_FLAGS += -EL
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJCOPY_FLAGS = -O binary
OBJDUMP = $(CROSS_COMPILE)objdump
OBJDUMP_FLAGS = -D
TARGET = start
OBJS = start.o main.o
OBJS += led.o

all: $(OBJS)
	$(LD) $(LD_FLAGS) $(OBJS) $(LIBCS) -o $(TARGET)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(TARGET) $(TARGET).bin
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(TARGET) > $(TARGET).dis
	@mkdir target
	@mv *.o $(TARGET) *.dis *.bin target

%.o:%.s
	$(CC) $(CC_FLAGS) $^
%.o:%.S
	$(CC) $(CC_FLAGS) $^
%.o:%.c
	$(CC) $(CC_FLAGS) $^

clean:
	@rm -rf *.o start.dis start start.bin
	@rm target -rf
	@echo "======== Clean target ========"
