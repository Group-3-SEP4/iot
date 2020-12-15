#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "configuration.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "secure_print.h"
#include "secure_print.h"

#define CLASS_NAME	"configuration.c"

#define MIN_CO2_FLAG 0
#define MAX_CO2_FLAG 1

typedef struct configuration {
	uint16_t temp;
	uint16_t co2Range[2]; // only min[0] and max[1]

} configuration;

static SemaphoreHandle_t _confMutex;


configuration_t configuration_create(void) {
	
	configuration_t self = pvPortMalloc(sizeof(configuration));
	if (NULL == self){
		return NULL;
	}
	_confMutex = xSemaphoreCreateMutex();
	
	self->temp = eeprom_read_word(DEF_MEMLOC_TEMP);
	self->co2Range[MIN_CO2_FLAG] = eeprom_read_word(DEF_MEMLOC_CO2_MIN);
	self->co2Range[MAX_CO2_FLAG] = eeprom_read_word(DEF_MEMLOC_CO2_MAX);
	if (DEF_PRINT_TO_TERMINAL){
		s_print("PRODUCTION", CLASS_NAME, "configuration_create: Read from EEPROM: %d, %d, %d", self->temp, self->co2Range[MIN_CO2_FLAG], self->co2Range[MAX_CO2_FLAG]);
	}
	return self;
}


uint16_t configuration_getDefaultTemperatur(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->temp;
		_xSemaphoreGive(_confMutex);
	}
	return _tempValue;
}


void configuration_setDefaultTemperatur(configuration_t self, uint16_t temp) {
	if (self->temp != temp) {
		if (_xSemaphoreTake(_confMutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) { // if conf. cant be run, wait forever.
			eeprom_write_word(DEF_MEMLOC_TEMP, temp);
			self->temp = temp;
			_xSemaphoreGive(_confMutex);
			if (DEF_PRINT_TO_TERMINAL){
				s_print("INFO", CLASS_NAME, "Set temp: %d", temp);
			}
		}
	}
}


uint16_t configuration_getMinCo2(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->co2Range[MIN_CO2_FLAG];
		xSemaphoreGive(_confMutex);
	}
	return _tempValue;
}


void configuration_setMinCo2(configuration_t self, uint16_t min) {

	if (self->co2Range[MIN_CO2_FLAG] != min) {
		if (xSemaphoreTake(_confMutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MIN, min);
			self->co2Range[MIN_CO2_FLAG] = min;
			xSemaphoreGive(_confMutex);
			if (DEF_PRINT_TO_TERMINAL){
				s_print("INFO", CLASS_NAME, "Set min co2: %d", min);
			}
		}
	}
}


uint16_t configuration_getMaxCo2(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->co2Range[MAX_CO2_FLAG];
		xSemaphoreGive(_confMutex);
	}
	return _tempValue;
}


void configuration_setMaxCo2(configuration_t self, uint16_t max) {
	if (self->co2Range[MAX_CO2_FLAG] != max) {
		if (xSemaphoreTake(_confMutex, DEF_WAIT_DEFAULT) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MAX, max);
			self->co2Range[MAX_CO2_FLAG] = max;
			xSemaphoreGive(_confMutex);
			if (DEF_PRINT_TO_TERMINAL){
				s_print("INFO", CLASS_NAME, "Set max co2: %d", max);
			}
		}
	}
}






