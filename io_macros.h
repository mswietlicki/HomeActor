#ifndef IO_MACROS_H_
#define IO_MACROS_H_

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

#endif