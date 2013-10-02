#!/bin/bash
rm program.o program.elf program.hex

avr-gcc -Os -DF_CPU=8000000UL -mmcu=attiny85 -c -o program.o main.c usiTwiSlave.c
avr-gcc -mmcu=attiny85 program.o -o program.elf
avr-objcopy -O ihex -R .eeprom program.elf program.hex
sudo avrdude -p t85 -c usbasp -P usb -U flash:w:$FILE.hex -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
