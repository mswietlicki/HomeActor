#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "i2c_slave.h"

int main(void){
	I2C_init(0x12); 
	
	sei();
	
	DDRB |= _BV(DDB4);
	
	while(1){
		if(i2c_buffer[1] == 1)
			PORTB |= _BV(PB4);
		else
			PORTB &= ~_BV(PB4);
		_delay_ms(300);
	}
	
	return 0;
}
