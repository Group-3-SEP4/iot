/*
*  Author: Jesper 273961
*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "configuration_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"

#define CLASS_NAME		"configuration_service.c"
#define MIN_CO2_FLAG	0
#define MAX_CO2_FLAG	1


typedef struct configuration {
	uint16_t temp;
	uint16_t co2_range[2]; // only min[0] and max[1]
} configuration;

static SemaphoreHandle_t _mutex;

configuration_t configuration_service_create(void) {
	
	configuration_t self = malloc(sizeof(configuration));
	if (NULL == self){
		return NULL;
	}
	_mutex = _xSemaphoreCreateMutex();
	
	self->temp = eeprom_read_word(DEF_MEMLOC_TEMP);
	self->co2_range[MIN_CO2_FLAG] = eeprom_read_word(DEF_MEMLOC_CO2_MIN);
	self->co2_range[MAX_CO2_FLAG] = eeprom_read_word(DEF_MEMLOC_CO2_MAX);
	s_print("INFO", CLASS_NAME, "EEPROM values: %d, %d, %d", self->temp, self->co2_range[MIN_CO2_FLAG], self->co2_range[MAX_CO2_FLAG]);
	return self;
}

uint16_t configuration_service_get_temperature(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake(_mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->temp;
		_xSemaphoreGive(_mutex);
	}
	return _tempValue;
}

void configuration_service_set_temperature(configuration_t self, uint16_t temp) {
	if (self->temp != temp) {
		if (_xSemaphoreTake(_mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) { // if config cant be run, wait forever.
			eeprom_write_word(DEF_MEMLOC_TEMP, temp);
			self->temp = temp;
			_xSemaphoreGive(_mutex);	
		}			
	}
}

uint16_t configuration_service_get_min_co2(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->co2_range[MIN_CO2_FLAG];
		xSemaphoreGive(_mutex);
	}
	return _tempValue;
}

void configuration_service_set_min_co2(configuration_t self, uint16_t min) {

	if (self->co2_range[MIN_CO2_FLAG] != min) {
		if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MIN, min);
			self->co2_range[MIN_CO2_FLAG] = min;
			xSemaphoreGive(_mutex);
		}
	}
}


uint16_t configuration_service_get_max_co2(configuration_t self) {
	uint16_t _tempValue = DEF_DEFAULT_NA_SENSOR;
	if (xSemaphoreTake(_mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		_tempValue = self->co2_range[MAX_CO2_FLAG];
		xSemaphoreGive(_mutex);
	}
	return _tempValue;
}

void configuration_service_set_max_co2(configuration_t self, uint16_t max) {
	if (self->co2_range[MAX_CO2_FLAG] != max) {
		if (xSemaphoreTake(_mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MAX, max);
			self->co2_range[MAX_CO2_FLAG] = max;
			xSemaphoreGive(_mutex);
		}
	}
}