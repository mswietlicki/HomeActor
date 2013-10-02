#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "usiTwiSlave.h"
#include "io_macros.h"

//==================

#define Out1_Bit 4
#define Out2_Bit 1
#define In1_Bit 3
#define In2_Bit 5

#define In1 (!GetBit(PINB,In1_Bit))
#define In2 (!GetBit(PINB,PB5))

//====================

#define REGISTER_SIZE 32
#define Default_I2C_Adress 0x10
#define ReadRegister(address) (Register[address])
#define slaveAddress Register[I2C_ADDRESS]

typedef enum
{
	I2C_ADDRESS			= 0x00,
	R_OUT1				= 0x01,
	R_OUT2				= 0x02,
	R_IN1				= 0x03,
	R_IN2				= 0x04
} register_t;

static uint8_t EEMEM eeprom_buffor[REGISTER_SIZE];
static uint8_t EEMEM is_first_run;
static uint8_t Register[REGISTER_SIZE] = {0};

void LoadBufforFromEEPROM(){
	if(eeprom_read_byte(&is_first_run)){
		eeprom_write_block(&Register[0], &eeprom_buffor[0], REGISTER_SIZE); //Reset EEPROM
		eeprom_write_byte(&eeprom_buffor[I2C_ADDRESS], Default_I2C_Adress); //Set default address

		eeprom_write_byte(&is_first_run, 0); //Zero first run flag
	}

	eeprom_read_block(&Register[0], &eeprom_buffor[0], REGISTER_SIZE);	//Read register from EEPROM
}

void WriteRegister(uint8_t pointer, uint8_t value, uint8_t persistent){
	Register[pointer] = value;
	if(persistent)
		eeprom_write_byte(&eeprom_buffor[pointer], value);
}

void Init_Register(){
	LoadBufforFromEEPROM();
}

//=======================

// A callback triggered when the i2c master attempts to read from a register.
uint8_t i2cReadFromRegister(uint8_t reg)
{
	return ReadRegister(reg);
}

// A callback triggered when the i2c master attempts to write to a register.
void i2cWriteToRegister(uint8_t reg, uint8_t value)
{
	WriteRegister(reg, value, 1);
}


void InOutLoop(register_t in_register, register_t out_register, uint8_t in_value){
	if(ReadRegister(in_register) != in_value)		//On button down
	{
		if(ReadRegister(out_register) > 0)
			WriteRegister(out_register, 0, 1);
		else
			WriteRegister(out_register, 1, 1);

		WriteRegister(in_register, in_value, 0);	//Move button value register
	}
}

int main(void){
	Init_Register();

	usiTwiSlaveInit(slaveAddress, i2cReadFromRegister, i2cWriteToRegister);
	
	sei();

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
