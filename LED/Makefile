
CROSS_COMPILE = arm-linux-

CC = $(CROSS_COMPILE)gcc
CC_FLAGS = -c -O3

LD = $(CROSS_COMPILE)ld
LD_FLAGS = -T start.lds

OBJCOPY = $(CROSS_COMPILE)objcopy
OBJCOPY_FLAGS = -O binary

OBJDUMP = $(CROSS_COMPILE)objdump
OBJDUMP_FLAGS = -D

TARGET = start

OBJS = start.o main.o led.o

all: $(OBJS)
	$(LD) $(LD_FLAGS) $(OBJS) -o $(TARGET)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(TARGET) $(TARGET).bin
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(TARGET) > $(TARGET).dis

%.o:%.s
	$(CC) $(CC_FLAGS) $^
%.o:%.S
	$(CC) $(CC_FLAGS) $^
%.o:%.c
	$(CC) $(CC_FLAGS) $^

clean:
	rm -rf *.o start.dis start start.bin
				





