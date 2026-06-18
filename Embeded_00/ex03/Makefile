MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMATER = arduino
BAUD = 115200

CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -Os -DF_CPU=$(F_CPU)
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

all : hex flash

hex : main.hex

main.bin : main.c
	$(CC) $(CFLAGS) -o main.bin main.c

main.hex : main.bin
	$(OBJCOPY) -O ihex -R .eeprom main.bin main.hex

flash : main.hex
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMATER) -b $(BAUD) -P /dev/ttyUSB0 -U flash:w:main.hex:i

clean :
	rm -f main.hex main.bin

.PHONY : all hex flash clean