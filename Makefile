# Output
FILE_ELF=main.elf
FILE_HEX=main.hex
FILE_SU=main.su

# Hardware
MCU_GCC=atmega328p
MCU_DUDE=m328p
F_CPU=20000000#Hz
PROG_DUDE=usbasp

AVRDUDE=avrdude -c$(PROG_DUDE) -p$(MCU_DUDE)

$(FILE_ELF): Makefile *.cpp *.hpp
	avr-gcc *.cpp -o$(FILE_ELF) -std=c++20 -mmcu=$(MCU_GCC) -DF_CPU=$(F_CPU)UL\
		-Os -maccumulate-args -mcall-prologues -mrelax -fwhole-program\
		-ffunction-sections -fdata-sections -fmerge-constants -Wl,--gc-sections\
		-fshort-enums -ffast-math\
		-Wall -Wextra -Werror -Wunused-parameter -Wno-unused-function -Wundef\
		-Wno-volatile -Wdouble-promotion -Wformat=2 -Wformat-truncation=2\
		-fno-common -fstack-usage

$(FILE_SU): $(FILE_ELF)

$(FILE_HEX): $(FILE_ELF)
	avr-objcopy -j .text -j .data -O ihex $(FILE_ELF) $(FILE_HEX)

.PHONY: clean upload fuse size

size: $(FILE_ELF) $(FILE_SU)
	cat $(FILE_SU)
	avr-size -C --mcu=$(MCU_GCC) $(FILE_ELF)

clean:
	-rm *.elf *.hex *.su

upload: $(FILE_HEX)
	$(AVRDUDE) -Uflash:w:$(FILE_HEX):i

fuse:
	$(AVRDUDE) -Ulfuse:w:0xff:m -Uhfuse:w:0xde:m -Uefuse:w:0xfc:m

