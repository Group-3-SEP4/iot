#pragma once
#include <stdint.h>

/*
 *
 *	for testing purpose
 * 
 */
uint16_t eeprom_read_word(uint16_t* addr);
void eeprom_write_word(uint16_t* addr, uint16_t value2);