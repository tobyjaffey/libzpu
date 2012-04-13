TARGET=asmtest

all: $(TARGET).bin

$(TARGET).elf: $(TARGET).S
	zpu-elf-as -o $(TARGET).elf $(TARGET).S

$(TARGET).bin: $(TARGET).elf
	zpu-elf-objcopy -j .text -j .data -j .fixed_vectors -O binary $(TARGET).elf $(TARGET).bin

run: $(TARGET).bin
	../zpusim/zpusim $(TARGET).bin

dis: $(TARGET).elf
	zpu-elf-objdump -D $(TARGET).elf

clean:
	rm -f $(TARGET).elf $(TARGET).bin

