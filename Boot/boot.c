#include <inttypes.h>
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "usiTwiSlave.h"
#include "../config.h"

enum
{
	PROGRAM_SIZE_H = 0,
	PROGRAM_SIZE_L = 1,
	PROGRAM_DATA = 2
}

uint16_t page = 0;
uint8_t buffer[2 * (SPM_PAGESIZE + 1)];
uint8_t index = 0;
uint8_t index_max = 2 * SPM_PAGESIZE; 
uint8_t state = PROGRAM_SIZE_H;
uint16_t program_size = 0;
uint16_t program_index = 0;

void (*main_entry_point)(void) = 0x0000;

inline void write_buffer_to_flash() 
{
	uint16_t i;
	const uint8_t* p = buffer;
	eeprom_busy_wait();

	boot_page_erase(page);
	boot_spm_busy_wait();

	for (i = 0; i < SPM_PAGESIZE; i += 2) {
		uint16_t w = *p++;
		w |= (*p++) << 8;
		boot_page_fill(page + i, w);
	}

	boot_page_write(page);
	boot_spm_busy_wait();
}

inline uint8_t i2cRead()
{
	return SPM_PAGESIZE;
}

inline void i2cWrite(uint8_t value)
{
	switch (state) {
	case PROGRAM_SIZE_H:
		program_size = value;
		state = PROGRAM_SIZE_L;
		break;
	case PROGRAM_SIZE_L:
		program_size |= value << 8;
		state = PROGRAM_DATA;
		break;
	case PROGRAM_DATA:
		buffer[index++] = value;
		program_index++;
		if(program_index == program_size)
		{
			main_entry_point();
		}
		if(index == SPM_PAGESIZE)
		{
			write_buffer_to_flash();
			page+=SPM_PAGESIZE;
			index = 0;
		}
		break;
	}
}

inline uint8_t is_update_ready()
{
	return eeprom_read_byte ((uint8_t *)UPDATE_READY );
}

int main(void)
{
	if (is_update_ready()) {
		usiTwiSlaveInit(slaveAddress, i2cRead, i2cWrite);
		sei();

		while(1) { }
	}
	main_entry_point();
}