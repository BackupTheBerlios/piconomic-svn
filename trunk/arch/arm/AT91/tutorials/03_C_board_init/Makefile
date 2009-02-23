#Basic Makefile

TARGET = C_board_init

# Default target
all: $(TARGET).elf $(TARGET).bin $(TARGET).sym $(TARGET).lss size

# Assemble *.S and create *.o object file
%.o : %.S
	arm-elf-gcc -mcpu=arm7tdmi -g -D__ASSEMBLY__ -c $<

# Compile *.c and create *.o object file
%.o : %.c
	arm-elf-gcc -mcpu=arm7tdmi -g -c $<

# Link *.o file(s) and create ELF file
%.elf: arm_crt0_tut03.S board.c
	arm-elf-gcc -T at91sam7s256-rom_tut03.lds -mcpu=arm7tdmi -nostartfiles -Wl,-Map=$(TARGET).map,--cref $^ -o $@

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
	rm -f $(TARGET).map
	rm -f $(TARGET).bin
	rm -f $(TARGET).sym
	rm -f $(TARGET).lss

