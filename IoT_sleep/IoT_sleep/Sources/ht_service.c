#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <hih8120.h>
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_task.h"
#include "ht_service.h"
#include "secure_print.h"

#define CLASS_NAME	"ht_service.c"


typedef struct ht_service {
	uint16_t humidity;
	int16_t temperature;
	SemaphoreHandle_t mutex;
	EventGroupHandle_t event_collect;
	EventGroupHandle_t event_ready;
}ht_service;


int16_t ht_service_get_temperature(ht_service_t service) {
	int16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_TEMP_READ) == pdTRUE)
	{
		tmp_value = service->temperature;
		_xSemaphoreGive(service->mutex);
	}
	return tmp_value;
}


uint16_t ht_service_get_humidity(ht_service_t service) {
	uint16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_HUM_READ) == pdTRUE)
	{
		tmp_value = service->humidity;
		_xSemaphoreGive(service->mutex);
	}
	return tmp_value;
}


inline void ht_service_measure(ht_service_t service){
	
	if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_HUM_TEMP_WRITE) == pdTRUE) // protect shared data
	{
		hih8120_wakeup();
		vTaskDelay(50);
		hih8120_measure();
		vTaskDelay(50);
		
		if(hih8120_isReady())
		{
			service->humidity = hih8120_getHumidity();
			service->temperature = hih8120_getTemperature();
			_xSemaphoreGive(service->mutex);
			if (_xEventGroupGetBits(service->event_collect) & DEF_BIT_DATA_COLLECT_HUM_TEMP) // checks eventMeasureStart bits
			{
				_xEventGroupClearBits(service->event_collect, DEF_BIT_DATA_COLLECT_HUM_TEMP); // clears eventMeasure bits
				_xEventGroupSetBits(service->event_ready, DEF_BIT_DATA_READY_HUM_TEMP); // sets eventDataReady bits
			}
			s_print("INFO", CLASS_NAME, "Current temperature and humidity: %i, %i", ht_service_get_temperature(service), ht_service_get_humidity(service));
		}
	}
}


void ht_service_task(void* pvParameters){


	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_HUM_TEMP;
		
	for (;;)
	{
		_vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		ht_service_measure((ht_service_t) pvParameters);
	}
	
}


ht_service_t ht_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready){
	
	ht_service_t service = malloc(sizeof(ht_service_t));
	
	if (service == NULL)
	return NULL;

	service->humidity = 0;
	service->temperature = 0;

	service->mutex = _xSemaphoreCreateMutex();

	service->event_collect = event_group_data_collect;
	service->event_ready = event_group_data_ready;

	hih8120_create();
	
	_xTaskCreate(
	ht_service_task,
	"ht_measure_task",
	DEF_STACK_HUM_TEMP,
	service,
	DEF_PRIORITY_TASK_HUM_TEMP,
	NULL
	);
	
	return service;
}


