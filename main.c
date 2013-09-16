#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "i2c_slave.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

int main(void){
	I2C_init(0x10); 
	
	sei();
	
	DDRB |= _BV(DDB0); 				//LED1
	DDRB &= ~_BV(DDB1);				//Button
	PORTB |= _BV(PB1);				//pull-up Button
	
	while(1){
	
		i2c_buffer[2] = !CHECK_BIT(PINB,PB1);
			
		if(i2c_buffer[1] == 1)
			PORTB |= _BV(PB0);
		else
			PORTB &= ~_BV(PB0);
			
		_delay_ms(50);
	}
	
	return 0;
}
