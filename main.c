#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "i2c_slave.h"

int main(void){
	I2C_init(0x10); 
	
	sei();
	
	while(1){
	}
	
	return 0;
}
