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
#include "co2_sensor.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "ht_sensor.h"
#include "secure_print.h"

#define CLASS_NAME	"ht_sensor.c"


typedef struct ht_sensor {
	uint16_t humidity;
	int16_t temperature;
}ht_sensor;

static SemaphoreHandle_t _mutex;

static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;

static EventBits_t _bitMeasureStart;
static EventBits_t _bitDataReady;


int16_t ht_getTemperature(ht_sensor_t sensor) {
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_TEMP_READ) == pdTRUE)
	{
		_tmpValue = sensor->temperature;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

uint16_t ht_getHumidity(ht_sensor_t sensor) {
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_HUM_READ) == pdTRUE)
	{
		_tmpValue = sensor->humidity;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

inline void ht_measure(ht_sensor_t sensor){
	
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
			if (_xEventGroupGetBits(_eventGroupMeasure) & _bitMeasureStart) // checks eventMeasureStart bits
			{
				_xEventGroupClearBits(_eventGroupMeasure, _bitMeasureStart); // clears eventMeasure bits
				_xEventGroupSetBits(_eventGroupDataReady, _bitDataReady); // sets eventDataReady bits
			}
			
			if (DEF_PRINT_TO_TERMINAL){
				s_print("INFO", CLASS_NAME, "Current temperature and humidity: %i, %i", ht_getTemperature(sensor), ht_getHumidity(sensor));
			}
		}

	}
	
}

void ht_task(void* pvParameters){


	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_HUM_TEMP;
		
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		ht_measure((ht_sensor_t) pvParameters);
	}
	
}

ht_sensor_t ht_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady){
	
	ht_sensor_t sensor = malloc(sizeof(ht_sensor_t));
	
	if (sensor == NULL)
	return NULL;

	sensor->humidity = 0;
	sensor->temperature = 0;

	_mutex = _xSemaphoreCreateMutex();

	_eventGroupMeasure = eventGroupMeasure;
	_eventGroupDataReady = eventGroupDataReady;

	_bitMeasureStart = DEF_BIT_MEASURE_START_HUM_TEMP;
	_bitDataReady = DEF_BIT_DATA_READY_HUM_TEMP;


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


