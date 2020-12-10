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


typedef struct ht_measurement {
	uint16_t humidity;
	int16_t temperature;
}ht_measurement;

static SemaphoreHandle_t _mutex;

static EventGroupHandle_t _event_group_data_collect;
static EventGroupHandle_t _event_group_data_ready;


int16_t ht_get_temperature(ht_t sensor) {
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_TEMP_READ) == pdTRUE)
	{
		_tmpValue = sensor->temperature;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

uint16_t ht_get_humidity(ht_t sensor) {
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_HUM_READ) == pdTRUE)
	{
		_tmpValue = sensor->humidity;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

inline void ht_measure(ht_t sensor){
	
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_HUM_TEMP_WRITE) == pdTRUE) // protect shared data
	{
		hih8120_wakeup();
		vTaskDelay(50);
		hih8120_measure();
		vTaskDelay(50);
		
		if(hih8120_isReady())
		{
			sensor->humidity = hih8120_getHumidity();
			sensor->temperature = hih8120_getTemperature();
			_xSemaphoreGive(_mutex);
			if (_xEventGroupGetBits(_event_group_data_collect) & DEF_BIT_MEASURE_START_HUM_TEMP) // checks eventMeasureStart bits
			{
				_xEventGroupClearBits(_event_group_data_collect, DEF_BIT_MEASURE_START_HUM_TEMP); // clears eventMeasure bits
				_xEventGroupSetBits(_event_group_data_ready, DEF_BIT_DATA_READY_HUM_TEMP); // sets eventDataReady bits
			}

			if (DEF_PRINT_TO_TERMINAL){
				printf("ht_measure: Current temperature and humidity: %i, %i\n", ht_get_temperature(sensor), ht_get_humidity(sensor));
			}
		}

	}
	
}

void ht_task(void* pv_parameters){


	TickType_t x_last_wake_time = xTaskGetTickCount();
	const TickType_t x_frequency = DEF_DELAY_TASK_HUM_TEMP;
		
	for (;;)
	{
		vTaskDelayUntil(&x_last_wake_time, x_frequency); // execution delay must be defined as first
		ht_measure((ht_t) pv_parameters);
	}
	
}

ht_t ht_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready){
	
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
	ht_task,
	"ht_measure_task",
	DEF_STACK_HUM_TEMP,
	sensor,
	DEF_PRIORITY_TASK_HUM_TEMP,
	NULL
	);
	
	return sensor;
}


