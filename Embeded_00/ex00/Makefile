MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMATER = arduino

CC = avr-gcc
CFLAGS = -Wall -mmcu=$(MCU) -DF_CPU=$(F_CPU) -nostdlib
OBJCOPY = avr-objcopy
AVRUDE = avrdude

all : hex flash

hex : main.hex

main.bin : main.c
	$(CC) $(CFLAGS) -o main.bin main.c

main.hex : main.bin
	$(OBJCOPY) -O ihex main.bin main.hex

flash : main.hex
	$(AVRUDE) -p $(MCU) -c $(PROGRAMMATER) -P /dev/ttyUSB0 -U flash:w:main.hex

clean :
	rm -f main.hex main.bin

.PHONY : all hex flash clean