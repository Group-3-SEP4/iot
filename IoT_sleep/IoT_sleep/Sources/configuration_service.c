#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <ATMEGA_FreeRTOS.h>
#include "configuration_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "secure_print.h"

#define CLASS_NAME	"configuration_service.c"

#define MIN_CO2_FLAG 0
#define MAX_CO2_FLAG 1

typedef struct configuration_service {
	uint16_t temp;
	uint16_t co2Range[2]; // only min[0] and max[1]
	SemaphoreHandle_t mutex;
} configuration_service_st;



configuration_service_t configuration_service_create(void) {
	
	configuration_service_t service = malloc(sizeof(configuration_service_st));
	if (NULL == service){
		return NULL;
	}
	service->mutex = _xSemaphoreCreateMutex();
	
	service->temp = eeprom_read_word(DEF_MEMLOC_TEMP);
	service->co2Range[MIN_CO2_FLAG] = eeprom_read_word(DEF_MEMLOC_CO2_MIN);
	service->co2Range[MAX_CO2_FLAG] = eeprom_read_word(DEF_MEMLOC_CO2_MAX);
	s_print("INFO", CLASS_NAME, "configuration_create: Read from EEPROM: %d, %d, %d", service->temp, service->co2Range[MIN_CO2_FLAG], service->co2Range[MAX_CO2_FLAG]);
	return service;
}


uint16_t configuration_service_get_temperature(configuration_service_t service) {
	uint16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake(service->mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		tmp_value = service->temp;
		_xSemaphoreGive(service->mutex);
	}
	return tmp_value;
}


void configuration_service_set_temperature(configuration_service_t service, uint16_t temp) {
	if (service->temp != temp) {
		if (_xSemaphoreTake(service->mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) { // if conf. cant be run, wait forever.
			eeprom_write_word(DEF_MEMLOC_TEMP, temp);
			service->temp = temp;
			_xSemaphoreGive(service->mutex);
			s_print("INFO", CLASS_NAME, "Set temp: %d", temp);
		}
	}
}


uint16_t configuration_service_get_min_co2(configuration_service_t service) {
	uint16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake(service->mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		tmp_value = service->co2Range[MIN_CO2_FLAG];
		_xSemaphoreGive(service->mutex);
	}
	return tmp_value;
}


void configuration_service_set_min_co2(configuration_service_t service, uint16_t min) {

	if (service->co2Range[MIN_CO2_FLAG] != min) {
		if (_xSemaphoreTake(service->mutex, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MIN, min);
			service->co2Range[MIN_CO2_FLAG] = min;
			_xSemaphoreGive(service->mutex);
			s_print("INFO", CLASS_NAME, "Set min co2: %d", min);
		}
	}
}


uint16_t configuration_service_get_max_co2(configuration_service_t service) {
	uint16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake(service->mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
		tmp_value = service->co2Range[MAX_CO2_FLAG];
		_xSemaphoreGive(service->mutex);
	}
	return tmp_value;
}


void configuration_service_set_max_co2(configuration_service_t service, uint16_t max) {
	if (service->co2Range[MAX_CO2_FLAG] != max) {
		if (_xSemaphoreTake(service->mutex, DEF_WAIT_DEFAULT) == pdTRUE) {
			eeprom_write_word(DEF_MEMLOC_CO2_MAX, max);
			service->co2Range[MAX_CO2_FLAG] = max;
			_xSemaphoreGive(service->mutex);
			s_print("INFO", CLASS_NAME, "Set max co2: %d", max);
		}
	}
}






