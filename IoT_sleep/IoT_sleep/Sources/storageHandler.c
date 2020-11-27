#include <avr/eeprom.h>
#include <stdint.h>
#include "storageHandler.h"
#include "definitions.h"



uint16_t storageHandler_readTemperature(){
	return eeprom_read_word(DEF_MEMLOC_TEMP);
}

void storangeHandler_writeTemperature(uint16_t value) {
	eeprom_write_word(DEF_MEMLOC_TEMP, value);
}

uint16_t storageHandler_readCo2Min(){
	return eeprom_read_word(DEF_MEMLOC_CO2_MIN);
}

void storangeHandler_writeCo2Min(uint16_t value) {
	eeprom_write_word(DEF_MEMLOC_CO2_MIN, value);
}

uint16_t storageHandler_readCo2Max(){
	return eeprom_read_word(DEF_MEMLOC_CO2_MAX);
}

void storangeHandler_writeCo2Max(uint16_t value) {
	eeprom_write_word(DEF_MEMLOC_CO2_MAX, value);
}



