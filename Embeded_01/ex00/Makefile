MCU = atmega328p # Microcontroller unit
F_CPU = 16000000UL # 16 Mhz
PROGRAMMER = arduino # Programmer Type
BAUD = 115200 # Baud Rate pour AVRDUDE

CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -Os -DF_CPU=$(F_CPU) 
OBJCOPY = avr-objcopy # bin en hex
AVRDUDE = avrdude # pour flasher le microcontroleur

all : hex flash 

hex : main.hex

main.bin : main.c
		$(CC) $(CFLAGS) -o main.bin main.c

main.hex : main.bin
		$(OBJCOPY) -O ihex -R .eeprom main.bin main.hex

flash : main.hex
		$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -b $(BAUD) -P /dev/ttyUSB0 -U flash:w:main.hex:i

clean :
		rm -f main.hex main.bin

.PHONY : all hex flash clean