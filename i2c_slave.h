#ifndef I2CSLAVE_H_
#define I2CSLAVE_H_

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

//For ATTiny85
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PB0
#define PORT_USI_SCL        PB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#define SET_USI_TO_SEND_ACK() \
{ \
	USIDR = 0; \
	DDR_USI |= (1 << PORT_USI_SDA); \
	USISR = \
	(0 << USI_START_COND_INT) | \
	(1 << USIOIF) | (1 << USIPF) | \
	(1 << USIDC)| \
	(0x0E << USICNT0); \
}

#define SET_USI_TO_READ_ACK() \
{ \
	DDR_USI &= ~(1 << PORT_USI_SDA); \
	USIDR = 0; \
	USISR = \
	(0 << USI_START_COND_INT) | \
	(1 << USIOIF) | \
	(1 << USIPF) | \
	(1 << USIDC) | \
	(0x0E << USICNT0); \
}

#define SET_USI_TO_TWI_START_CONDITION_MODE() \
{ \
	USICR = \
	(1 << USISIE) | (0 << USIOIE) | \
	(1 << USIWM1) | (0 << USIWM0) | \
	(1 << USICS1) | (0 << USICS0) | (0 << USICLK) | \
	(0 << USITC); \
	USISR = \
	(0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | \
	(1 << USIDC) | (0x0 << USICNT0); \
}

#define SET_USI_TO_SEND_DATA() \
{ \
	DDR_USI |=  (1 << PORT_USI_SDA); \
	USISR    =  \
	(0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | \
	(1 << USIDC) | \
	(0x0 << USICNT0); \
}

#define SET_USI_TO_READ_DATA() \
{ \
	DDR_USI &= ~(1 << PORT_USI_SDA); \
	USISR    = \
	(0 << USI_START_COND_INT) | (1 << USIOIF) | \
	(1 << USIPF) | (1 << USIDC) | \
	(0x0 << USICNT0); \
}

#define ReadRegister(address) (Register[address])

#define MAX_Register 32
#define Default_I2C_Adress 0x10

typedef enum
{
	USI_SLAVE_CHECK_ADDRESS                = 0x00,
	USI_SLAVE_SEND_DATA                    = 0x01,
	USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA = 0x02,
	USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA   = 0x03,
	USI_SLAVE_REQUEST_DATA                 = 0x04,
	USI_SLAVE_GET_DATA_AND_SEND_ACK        = 0x05
} overflowState_t;

typedef enum
{
	I2C_ADDRESS			= 0x00,
	R_OUT1				= 0x01,
	R_OUT2				= 0x02,
	R_IN1				= 0x03,
	R_IN2				= 0x04
} register_t;

static uint8_t slaveAddress;
static volatile overflowState_t overflowState;

static uint8_t EEMEM eeprom_buffor[MAX_Register];
static uint8_t is_first_run EEMEM;
static uint8_t Register[MAX_Register] = {0};
static uint8_t Register_pointer;

void LoadBufforFromEEPROM(){
	if(is_first_run){
		eeprom_write_block(&Register[0], &eeprom_buffor[0], MAX_Register); //Reset EEPROM
		eeprom_write_byte(&eeprom_buffor[I2C_ADDRESS], Default_I2C_Adress); //Set default address
		eeprom_write_byte(&is_first_run, 0); //Zero first run flag
	}else{
		eeprom_read_block(&Register[0], &eeprom_buffor[0], MAX_Register);	//Read register from EEPROM
	}
}

void WriteRegister(uint8_t pointer, uint8_t value){
	Register[pointer] = value;
	eeprom_write_byte(&eeprom_buffor[pointer], value);
}

void I2C_init(){
	LoadBufforFromEEPROM();
	I2C_init(ReadRegister(I2C_ADDRESS));
}

void I2C_init(uint8_t ownAddress)
{
	Register_pointer = 0xFF;
	slaveAddress = ownAddress;

	DDR_USI |= (1 << PORT_USI_SCL) | (1 << PORT_USI_SDA); // Set SCL and SDA as output
	PORT_USI |= (1 << PORT_USI_SCL); // set SCL high
	PORT_USI |= (1 << PORT_USI_SDA); // set SDA high
	DDR_USI &= ~(1 << PORT_USI_SDA); // Set SDA as input

	USICR =
		(1 << USISIE) | // enable Start Condition Interrupt
		(0 << USIOIE) | // disable Overflow Interrupt
		(1 << USIWM1) | (0 << USIWM0) | // set USI in Two-wire mode, no USI Counter overflow hold
		(1 << USICS1) | (0 << USICS0) | (0 << USICLK) | // Shift Register Clock Source = external, positive edge 4-Bit Counter Source = external, both edges
		(0 << USITC); // no toggle clock-port pin

	USISR = (1 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC); // clear all interrupt flags and reset overflow counter
}

ISR(USI_START_VECTOR)
{
	overflowState = USI_SLAVE_CHECK_ADDRESS; // set default starting conditions for new TWI package
	DDR_USI &= ~(1 << PORT_USI_SDA); // set SDA as input

	while ((PIN_USI & (1 << PIN_USI_SCL)) && !((PIN_USI & (1 << PIN_USI_SDA)))); // SCL is high and SDA is low

	if (!(PIN_USI & (1 << PIN_USI_SDA)))
	{ // REPETED START
		USICR =
			(1 << USISIE) | // keep Start Condition Interrupt enabled to detect RESTART
			(1 << USIOIE) | // enable Overflow Interrupt
			(1 << USIWM1) | (1 << USIWM0) | // set USI in Two-wire mode, hold SCL low on USI Counter overflow
			(1 << USICS1) | (0 << USICS0) | (0 << USICLK) | // Shift Register Clock Source = external, positive edge 4-Bit Counter Source = external, both edges
			(0 << USITC); // no toggle clock-port pin
	}else{ //AFTER STOP
		USICR =
			(1 << USISIE) | // enable Start Condition Interrupt
			(0 << USIOIE) | // disable Overflow Interrupt
			(1 << USIWM1) | (0 << USIWM0) | // set USI in Two-wire mode, no USI Counter overflow hold
			(1 << USICS1) | (0 << USICS0) | (0 << USICLK) | // Shift Register Clock Source = external, positive edge 4-Bit Counter Source = external, both edges
			(0 << USITC); //no toggle clock-port pin
	}

	USISR =
		(1 << USI_START_COND_INT) | (1 << USIOIF) | //clear interrupt flags - resetting the Start Condition Flag will release SCL
		(1 << USIPF) |(1 << USIDC) |
		(0x0 << USICNT0); // set USI to sample 8 bits (count 16 external SCL pin toggles)
} 

ISR(USI_OVERFLOW_VECTOR)
{
	uint8_t data = 0;
	switch (overflowState)
	{
	case USI_SLAVE_CHECK_ADDRESS:
		if ((USIDR == 0) || ((USIDR >> 1) == slaveAddress))
		{
			if (USIDR & 0x01)
				overflowState = USI_SLAVE_SEND_DATA;
			else
			{
				overflowState = USI_SLAVE_REQUEST_DATA;
				Register_pointer = 0xFF;
			}
			SET_USI_TO_SEND_ACK();
		}
		else
			SET_USI_TO_TWI_START_CONDITION_MODE();
		break;

	case USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA:
		if (USIDR)
		{
			SET_USI_TO_TWI_START_CONDITION_MODE();
			return;
		}
	case USI_SLAVE_SEND_DATA:
		if (Register_pointer == 0xFF)		// No buffer position given, set buffer address to 0
			Register_pointer = 0;
		USIDR = ReadRegister(Register_pointer++);

		overflowState = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
		SET_USI_TO_SEND_DATA();
		break;

	case USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA:
		overflowState = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
		SET_USI_TO_READ_ACK();
		break;

	case USI_SLAVE_REQUEST_DATA:
		overflowState = USI_SLAVE_GET_DATA_AND_SEND_ACK;
		SET_USI_TO_READ_DATA();
		break;

	case USI_SLAVE_GET_DATA_AND_SEND_ACK:
		data = USIDR;
		if (Register_pointer == 0xFF)		// First access, read buffer position
			if (data < MAX_Register)		// Check if address within buffer size
				Register_pointer = data;		// Set position as received
			else
				Register_pointer = 0;			// Set address to 0		
		else
		{
			WriteRegister(Register_pointer++, data);
		}

		overflowState = USI_SLAVE_REQUEST_DATA;
		SET_USI_TO_SEND_ACK();
		break;
	} 

	if(Register_pointer != 0xFF && Register_pointer >= MAX_Register)
		Register_pointer = 0;
}
#endif

