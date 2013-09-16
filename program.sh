#!/bin/bash
if [ -z $1 ]; then
 echo "Usage $0 <source.c>"
 exit
fi
FILE=$(echo $1 | sed 's/\..*//')
rm $FILE.o $FILE.elf $FILE.hex
avr-gcc -Os -DF_CPU=1000000UL -mmcu=attiny85 -c -o $FILE.o $1
avr-gcc -mmcu=attiny85 $FILE.o -o $FILE.elf
avr-objcopy -O ihex -R .eeprom $FILE.elf $FILE.hex
sudo avrdude -p t85 -c usbasp -P usb -U flash:w:$FILE.hex