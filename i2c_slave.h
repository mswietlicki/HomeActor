#ifndef I2CSLAVE_H_
#define I2CSLAVE_H_

#include <inttypes.h>
#include <compat/twi.h>
#include <avr/interrupt.h>

#define TWI_ON()  {  TWCR=0x45; }
#define TWI_OFF() {  TWCR=0x00; }
#define TWI_PAUSE() { TWCR=0x05; }

#define MAX_I2C_BUFFER 32
static unsigned char i2c_buffer[MAX_I2C_BUFFER];
static unsigned char i2c_pointer;
static unsigned char i2c_is_pointer=0;

void I2C_init(unsigned char address_gce)
{
	TWAR = address_gce << 1;
	TWI_ON();
}

ISR(TWI_vect)
{
	switch(TW_STATUS)
    {
		case TW_SR_SLA_ACK:              // Slave receiver acknowledge
		case TW_SR_ARB_LOST_SLA_ACK:     // Arbitration lost
			TWCR = 0b11000101;           // TWINT, TWEA, TWEN, TWIE
			i2c_is_pointer = 1;
			break;
		case TW_SR_DATA_ACK:
			if(i2c_is_pointer){
				i2c_pointer = TWDR;
				i2c_is_pointer = 0;
			}
			else {                       
				i2c_buffer[i2c_pointer++] = TWDR;
			}
			TWCR = 0b11000101;           // TWINT, TWEA, TWEN, TWIE 
			break;
		case TW_SR_STOP:
			TWCR = 0b11000101;           // TWINT, TWEA, TWEN, TWIE
			break;
		case TW_ST_SLA_ACK:
		case TW_ST_ARB_LOST_SLA_ACK:
			TWDR = i2c_buffer[i2c_pointer++]; 
			TWCR = 0b11000101;           // TWINT, TWEA, TWEN, TWIE 
			break;
		case TW_ST_DATA_ACK:
			TWDR = i2c_buffer[i2c_pointer++]; 
		case TW_ST_DATA_NACK:
			TWCR = 0b11000101;           // TWINT, TWEA, TWEN, TWIE 
			break;
		case TW_BUS_ERROR:
			TWCR = 0b11010101;           // TWINT, TWEA, TWSTO, TWEN, TWIE
			break;
		default:
			TWCR = 0b11000101;           // TWINT, TWEA, TWEN, TWIE
     }

 if(i2c_pointer >= MAX_I2C_BUFFER)
     i2c_pointer = 0;

}

#endif