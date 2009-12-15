#Basic Makefile

TARGET = AsmAdvanced


# Default target
all: $(TARGET).elf $(TARGET).bin $(TARGET).sym $(TARGET).lss size

# Assemble *.S and create *.o object file
%.o : %.S
	arm-elf-gcc -mcpu=arm7tdmi -g -D__ASSEMBLY__ -c $<

# Link *.o file(s) and create ELF file
%.elf: $(TARGET).o
	arm-elf-gcc -mcpu=arm7tdmi -nostartfiles -Ttext=0 $^ -o $@

# Create binary from ELF file
%.bin: %.elf
	arm-elf-objcopy -O binary $< $@

# Create extended assembly listing file
%.lss: %.elf
	arm-elf-objdump -h -S $< > $@

# Create symbol file report
%.sym: %.elf
	arm-elf-nm -n $< > $@

# Report section sizes
size : $(TARGET).elf
	arm-elf-size $<

# Delete created files
clean:
	rm -f $(TARGET).o
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin
	rm -f $(TARGET).sym
	rm -f $(TARGET).lss


