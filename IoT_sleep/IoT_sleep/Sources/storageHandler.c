#include <avr/eeprom.h>
#include "storageHandler.h"

// ATmega2560 contains 4096 bytes.
#define MEMLOC_TEMPERATURE 0
#define MEMLOC_CO2_MIN 4
#define MEMLOC_CO2_MAX 8



void storeDefaultTemperature(uint16_t temp) {
	eeprom_write_float()
}

void storeMinCo2(uint16_t min) {
	
}

void storeMaxCo2(uint16_t max) {
	
}