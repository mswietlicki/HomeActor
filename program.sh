#!/bin/bash
MAIN='main.c'
FILE=$(echo $MAIN | sed 's/\..*//')
rm $FILE.o $FILE.elf $FILE.hex

avr-gcc -Os -DF_CPU=8000000UL -mmcu=attiny85 -c -o $FILE.o $MAIN
avr-gcc -mmcu=attiny85 $FILE.o -o $FILE.elf
avr-objcopy -O ihex -R .eeprom $FILE.elf $FILE.hex
sudo avrdude -p t85 -c usbasp -P usb -U flash:w:$FILE.hex -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
