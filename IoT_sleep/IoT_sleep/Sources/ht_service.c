#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include <hih8120.h>
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "ht_service.h"

#define CLASS_NAME	"ht_service.c"

typedef struct ht_measurement {
	uint16_t humidity;
	int16_t temperature;
}ht_measurement;

static SemaphoreHandle_t _mutex;

static EventGroupHandle_t _event_group_data_collect;
static EventGroupHandle_t _event_group_data_ready;


int16_t ht_service_get_temperature(ht_t sensor) {
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_T_READ) == pdTRUE)
	{
		_tmpValue = sensor->temperature;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

uint16_t ht_service_get_humidity(ht_t sensor) {
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_H_READ) == pdTRUE)
	{
		_tmpValue = sensor->humidity;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

void ht_service_measure(ht_t sensor){
	
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_HT_WRITE) == pdTRUE) // protect shared data
	{
		hih8120_wakeup();
		vTaskDelay(DEF_DELAY_DRIVER_HT);
		hih8120_measure();
		vTaskDelay(DEF_DELAY_DRIVER_HT);
		
		if(hih8120_isReady())
		{
			sensor->humidity = hih8120_getHumidity();
			sensor->temperature = hih8120_getTemperature();
			s_print("INFO", CLASS_NAME, "Temp: %i, Hum: %i ", sensor->temperature, sensor->humidity);
			//s_print("Free stack: %d, minimum heap: %d",xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
			s_print("INFO", CLASS_NAME, "Stack high water mark: %d", uxTaskGetStackHighWaterMark(NULL));
			s_print("INFO", CLASS_NAME, "Current temperature and humidity: %i, %i", ht_service_get_temperature(sensor), ht_service_get_humidity(sensor));
			
			_xSemaphoreGive(_mutex);
			if (_xEventGroupGetBits(_event_group_data_collect) & DEF_BIT_DATA_COLLECT_HT) // checks eventMeasureStart bits
			{
				_xEventGroupClearBits(_event_group_data_collect, DEF_BIT_DATA_COLLECT_HT); // clears eventMeasure bits
				_xEventGroupSetBits(_event_group_data_ready, DEF_BIT_DATA_READY_HT); // sets eventDataReady bits
			}


			
			
		} else {
			s_print("WARNING", CLASS_NAME, "hih8120 is not ready");
		}
	}
}

void ht_service_task(void* pv_parameters){

	TickType_t x_last_wake_time = xTaskGetTickCount();
	const TickType_t x_frequency = DEF_DELAY_TASK_HT;
		
	for (;;)
	{
		vTaskDelayUntil(&x_last_wake_time, x_frequency); // execution delay must be defined as first
		ht_service_measure((ht_t) pv_parameters);
	}
}

ht_t ht_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready){
	
	ht_t sensor = malloc(sizeof(ht_t));
	
	if (sensor == NULL)
	return NULL;

	sensor->humidity = 0;
	sensor->temperature = 0;

	_mutex = _xSemaphoreCreateMutex();

	_event_group_data_collect = event_group_data_collect;
	_event_group_data_ready = event_group_data_ready;

	hih8120_create();
	
	xTaskCreate(
	ht_service_task,
	"ht_measure_task",
	DEF_STACK_HT,
	sensor,
	DEF_PRIORITY_TASK_HT,
	NULL
	);
	
	return sensor;
}


