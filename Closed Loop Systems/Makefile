GCC_DIR =  /opt/ti/mspgcc/bin
SUPPORT_FILE_DIRECTORY = /opt/ti/mspgcc/include

# Please set your device here
DEVICE  = msp430f5529
CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb
OBJCOPY = $(GCC_DIR)/msp430-elf-objcopy
FLASH   = mspflash

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O2 -g -Werror
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -T $(DEVICE).ld

all: program

temp.o: temp.c temp.h thermistor_10k.h
	$(CC) $(CFLAGS) -c temp.c -o $@

uart.o: uart.c uart.h
	$(CC) $(CFLAGS) -c uart.c -o $@

main.o: main.c uart.h temp.h
	$(CC) $(CFLAGS) -c main.c -o $@

${DEVICE}.elf: main.o uart.o temp.o
	$(CC) $(CFLAGS) $(LFLAGS) main.o uart.o temp.o -o $(DEVICE).elf

${DEVICE}.hex: ${DEVICE}.elf
	$(OBJCOPY) -O ihex $(DEVICE).elf $(DEVICE).hex

program: ${DEVICE}.hex
	#$(FLASH) -n $(DEVICE) -w $(DEVICE).hex -v -z '[VCC]'
	$(FLASH) -w $(DEVICE).hex -v -z '[VCC]'

debug: ${DEVICE}.elf
	#$(GDB) $(DEVICE).elf
	$(GDB) -x debug.gdb

clean:
	rm *.o *.elf *.hex
