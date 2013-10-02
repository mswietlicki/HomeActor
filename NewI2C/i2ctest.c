#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include "usiTwiSlave.h"

#define I2C_SLAVE_ADDR  0x26


// A callback triggered when the i2c master attempts to read from a register.
uint8_t i2cReadFromRegister(uint8_t reg)
{
	switch (reg)
	{
		case 0: 
			return 10;
		case 1:
			return adcRead();
		default:
			return 0xff;
	}
}

// A callback triggered when the i2c master attempts to write to a register.
void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	switch (reg)
	{
		case 2: 
			i2cReg2 = value;
			break;
		case 3:
			i2cReg3 = value;
			break;
	}
}

int main()
{
	usiTwiSlaveInit(I2C_SLAVE_ADDR, i2cReadFromRegister, i2cWriteToRegister);

	sei();

	while (1)
	{

	}
}
