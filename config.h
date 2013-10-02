#ifndef CONFIG_H_
#define CONFIG_H_

#define REGISTER_SIZE 32
#define Default_I2C_Adress 0x10

typedef enum
{
	I2C_ADDRESS			= 0x00,
	UPDATE_READY		= 0x01,
	R_OUT1				= 0x02,
	R_OUT2				= 0x03,
	R_IN1				= 0x04,
	R_IN2				= 0x05
} register_t;

#endif