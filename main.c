#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "i2c_slave.h"

#define GetBit(var,pos) ((var) & (1<<(pos)))
#define SetBit(var,pos,val) if(val) var &= ~_BV(pos); else var |= _BV(pos)

#define SetPinOutput(pin) \
{ \
	DDRB |= _BV(pin); \
}

#define SetPinInput(pin, pullup) \
{ \
	DDRB &= ~_BV(pin); \
	SetBit(PORTB, pin, pullup); \
}

#define Out1_Bit 4;
#define Out2_Bit 1;
#define In1_Bit 3;
#define In2_Bit 5;

#define In1 (!GetBit(PINB,In1_Bit))
#define In2 (!GetBit(PINB,PB5))

void InOutLoop(register_t in_register, register_t out_register, uint8_t in_value){
	if(ReadRegister(in_register) != in_value)		//On button down
	{
		if(ReadRegister(out_register) > 0)
			WriteRegister(out_register, 0);
		else
			WriteRegister(out_register, 1);

		WriteRegister(in_register, in_value);	//Move button value register
	}
}

int main(void){
	sei();
	I2C_init(); 

	SetPinOutput(Out1_Bit);
	SetPinOutput(Out2_Bit);

	SetPinInput(In1_Bit, 1);
	SetPinInput(In2_Bit, 1);

	while(1){
		InOutLoop(R_IN1, R_OUT1, In1);
		InOutLoop(R_IN2, R_OUT2, In2);

		SetBit(PORTB, Out1_Bit, ReadRegister(R_OUT1));	//Set bit PB0 of PORTB to ROut1
		SetBit(PORTB, Out2_Bit, ReadRegister(R_OUT2));

		_delay_ms(50);
	}
	return 0;
}
