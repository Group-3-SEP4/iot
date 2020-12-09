#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include "co2_sensor.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"

typedef struct co2_sensor {
	uint16_t value;
} co2_sensor;


static SemaphoreHandle_t _co2_mutex;

static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;

static EventBits_t _bitMeasureStart;
static EventBits_t _bitDataReady;


uint16_t co2_getMeasurement(co2_sensor_t sensor){
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_co2_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE)
	{
		_tmpValue = sensor->value;
		_xSemaphoreGive(_co2_mutex);
	}
	return _tmpValue;
}


inline void co2_measure(co2_sensor_t sensor){
	
	mh_z19_returnCode_t _returnCode = mh_z19_takeMeassuring();
	vTaskDelay(DEF_DELAY_DRIVER_CO2); // delay must be placed here between takeMessuring() and check, since it takes time for the driver to measure (Async). If moved/deleted value will be 0 ppm.
	if(_returnCode == MHZ19_OK)
	{
		if (_xSemaphoreTake (_co2_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE) // protect shared data
		{
			mh_z19_getCo2Ppm(&sensor->value);
			_xSemaphoreGive(_co2_mutex);
			
			if (_xEventGroupGetBits(_eventGroupMeasure) & _bitMeasureStart) // checks eventMeasureStart bits
			{	
				_xEventGroupClearBits(_eventGroupMeasure, _bitMeasureStart); // clears eventMeasure bits
				_xEventGroupSetBits(_eventGroupDataReady, _bitDataReady); // sets eventDataReady bits
			}
			if (DEF_PRINT_TO_TERMINAL){
				printf("Current CO2: %i\n", co2_getMeasurement(sensor));
			}
		}
	}
}


void co2_task_measure(void* pvParameters){
	
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_CO2;
	
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first 
		co2_measure((co2_sensor_t) pvParameters);
	}
}


co2_sensor_t co2_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady){
	
	co2_sensor_t _sensor = malloc(sizeof(co2_sensor));
	if (NULL == _sensor){
		return NULL;
	}
	
	_sensor->value = DEF_DEFAULT_NA_SENSOR;	
	
	_co2_mutex = xSemaphoreCreateMutex();
	
	_eventGroupMeasure = eventGroupMeasure;
	_eventGroupDataReady = eventGroupDataReady;
	
	_bitMeasureStart = DEF_BIT_MEASURE_START_CO2;
	_bitDataReady = DEF_BIT_DATA_READY_CO2;
	
	
	mh_z19_create(MH_Z19_USART, NULL); 
	
	xTaskCreate(
		co2_task_measure,		/* Function that implements the task. */
		"CO2 Sensor task",		/* Text name for the task. */
		DEF_STACK_CO2,			/* Stack size in words, not bytes. */
		_sensor,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_CO2,	/* Priority at which the task is created. */
		NULL					/* Used to pass out the created task's handle. */
	);
	
	return _sensor;
}