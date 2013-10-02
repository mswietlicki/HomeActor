#include <inttypes.h>
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "usiTwiSlave.h"
#include "../config.h"


uint16_t page = 0;
uint8_t buffer[2 * (SPM_PAGESIZE + 1)];

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

inline void copy_program()
{
		write_buffer_to_flash();
		page+=SPM_PAGESIZE;
}

int main(void) {
	
	if (update_) {
		copy_program();
	}
	main_entry_point();
}