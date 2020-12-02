#include <stdint.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "configuration.h"
#include "definitions.h"

#define MIN_CO2 0
#define MAX_CO2 1

typedef struct configuration {
	uint16_t temp;
	uint16_t co2Range[2];

} configuration;

static SemaphoreHandle_t _confMutex;


configuration_t configuration_create(void) {
	
	configuration_t self = malloc(sizeof(configuration));
	if (NULL == self){
		return NULL;
	}
	_confMutex = xSemaphoreCreateMutex();
	
	self->temp = eeprom_read_word(DEF_MEMLOC_TEMP);
	self->co2Range[MIN_CO2] = eeprom_read_word(DEF_MEMLOC_CO2_MIN);
	self->co2Range[MAX_CO2] = eeprom_read_word(DEF_MEMLOC_CO2_MAX);
	
	return self;
}

uint16_t configuration_getDefaultTemperatur(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->temp;
		xSemaphoreGive(_confMutex);
	}
	return _tempValue;
}

void configuration_setDefaultTemperatur(configuration_t self, uint16_t temp) {
	if (self->temp != temp) {
		if (xSemaphoreTake(_confMutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) { // if conf. cant be run, wait forever.
			eeprom_write_word(DEF_MEMLOC_TEMP, temp);
			self->temp = temp;
			xSemaphoreGive(_confMutex);	
		}			
	}
}

uint16_t configuration_getMinCo2(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->co2Range[MIN_CO2];
		xSemaphoreGive(_confMutex);
	}
	return _tempValue;
}

void configuration_setMinCo2(configuration_t self, uint16_t min) {
	if (self->co2Range[MIN_CO2] != min) {
		if (xSemaphoreTake(_confMutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MIN, min);
			self->co2Range[MIN_CO2] = min;
			xSemaphoreGive(_confMutex);
		}		
	}
}


uint16_t configuration_getMaxCo2(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->co2Range[MAX_CO2];
		xSemaphoreGive(_confMutex);
	}
	return _tempValue;
}

void configuration_setMaxCo2(configuration_t self, uint16_t max) {
	if (self->co2Range[MAX_CO2] != max) {
		if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MAX, max);
			self->co2Range[MAX_CO2] = max;
			xSemaphoreGive(_confMutex);
		}
	}
}