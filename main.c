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

#define RIn1 (i2c_buffer[8])
#define ROut1 (i2c_buffer[0])

#define SetBit(var,pos,val) if(val) var &= ~_BV(pos); else var |= _BV(pos)

int main(void){
	I2C_init(0x10); 
	
	sei();
	
	DDRB |= _BV(DDB0); 				//LED1
	DDRB &= ~_BV(DDB1);				//Button
	PORTB |= _BV(PB1);				//pull-up Button
	
	while(1){
		if(In1 && RIn1 != In1)		//On button down
			ROut1 ^= 1;
			
		RIn1 = In1;					//Move button value register
		
		SetBit(PORTB, PB0, ROut1);	//Set bit PB0 of PORTB to ROut1
		
		_delay_ms(50);
	}
	return 0;
}
