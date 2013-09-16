#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "i2c_slave.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define In1 (!CHECK_BIT(PINB,PB1))

#define RIn1 (i2c_buffer[2])
#define ROou1 (i2c_buffer[1])

int main(void){
	I2C_init(0x10); 
	
	sei();
	
	DDRB |= _BV(DDB0); 				//LED1
	DDRB &= ~_BV(DDB1);				//Button
	PORTB |= _BV(PB1);				//pull-up Button
	
	while(1){
		if(RIn1 != In1)
			ROut1 ^= 1;
			
		RIn1 = In1;
		
		if(ROut1)
			PORTB &= ~_BV(PB0);		//LED ON
		else
			PORTB |= _BV(PB0);		//LED OFF
			
		_delay_ms(50);
	}
	
	return 0;
}
