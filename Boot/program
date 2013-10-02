#!/bin/bash
rm main.o usiTwiSlave.o main.elf main.hex

avr-gcc -Os -DF_CPU=8000000UL -mmcu=attiny85 -c main.c usiTwiSlave.c
avr-gcc -mmcu=attiny85 -o main.elf main.o usiTwiSlave.o
avr-objcopy -O ihex -R .eeprom main.elf main.hex
sudo avrdude -p t85 -c usbasp -P usb -U flash:w:main.hex -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
