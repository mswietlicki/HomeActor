#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "i2c_slave.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define In1 (!CHECK_BIT(PINB,PB4))

#define SetBit(var,pos,val) if(val) var &= ~_BV(pos); else var |= _BV(pos)

void InOutLoop(register_t in_register, register_t out_register, uint8_t in_value){
	if(ReadRegister(in_register) != in_value)		//On button down
		if(ReadRegister(out_register) > 0)
			WriteRegister(out_register, 0);
		else
			WriteRegister(out_register, 1);
	WriteRegister(in_register, in_value);	//Move button value register
}

int main(void){
	sei();
	I2C_init(); 

	DDRB |= _BV(DDB3) | _BV(DDB1); 				//OUT1 and OUT2
	DDRB &= ~_BV(DDB4);				//Button
	PORTB |= _BV(PB4);				//pull-up Button

	while(1){
		InOutLoop(R_IN1, R_OUT1, In1);



		SetBit(PORTB, PB3, ReadRegister(R_OUT1));	//Set bit PB0 of PORTB to ROut1
		SetBit(PORTB, PB1, ReadRegister(R_OUT2));

		_delay_ms(50);
	}
	return 0;
}
