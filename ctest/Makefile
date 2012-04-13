TARGET=ctest

LDFLAGS=-Wl,--relax -Wl,--gc-sections -phi -Wl,-Map,$(TARGET).map
CFLAGS=-g -Os -Wall -I../libzpu -I../zpusim -I. -phi

all: $(TARGET).bin

$(TARGET).o: $(TARGET).c
	zpu-elf-gcc $(CFLAGS) -c -o $(TARGET).o $(TARGET).c

$(TARGET).elf: $(TARGET).o
	zpu-elf-gcc $(CFLAGS) -o $(TARGET).elf $(TARGET).o

$(TARGET).bin: $(TARGET).elf
	zpu-elf-objcopy -j .text -j .data -j .fixed_vectors -j .rodata -O binary $(TARGET).elf $(TARGET).bin
	zpu-elf-size $(TARGET).elf

run: $(TARGET).bin
	../zpusim/zpusim $(TARGET).bin

dis: $(TARGET).elf
	zpu-elf-objdump -S -D $(TARGET).elf

clean:
	rm -f $(TARGET).elf $(TARGET).bin *.o *.map

