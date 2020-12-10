/*
 *  Author: Jacob 273962
 */ 
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include "co2_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_task.h"

typedef struct co2_measurement {
	uint16_t ppm;
} co2_measurement;

static SemaphoreHandle_t _mutex;

static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;

static EventBits_t _bitMeasureStart;
static EventBits_t _bitDataReady;


uint16_t co2_service_get_measurement(co2_service_t sensor){
	uint16_t _tmpValue = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE)
	{
		_tmpValue = sensor->ppm;
		_xSemaphoreGive(_mutex);
	}
	return _tmpValue;
}

inline void co2_service_measure(co2_service_t sensor){
	
	mh_z19_returnCode_t _returnCode = mh_z19_takeMeassuring();
	_vTaskDelay(DEF_DELAY_DRIVER_CO2); // delay must be placed here between takeMessuring() and check, since it takes time for the driver to measure (Async). If moved/deleted value will be 0 ppm.
	if(_returnCode == MHZ19_OK)
	{
		if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE) // protect shared data
		{
			mh_z19_getCo2Ppm(&sensor->ppm);
			_xSemaphoreGive(_mutex);
			
			if (_xEventGroupGetBits(_eventGroupMeasure) & _bitMeasureStart) // checks eventMeasureStart bits
			{
				_xEventGroupClearBits(_eventGroupMeasure, _bitMeasureStart); // clears eventMeasure bits
				_xEventGroupSetBits(_eventGroupDataReady, _bitDataReady); // sets eventDataReady bits
			}
			if (DEF_PRINT_TO_TERMINAL){
			printf("Current ppm: %i\n", co2_service_get_measurement(sensor)); // only for visual verification in terminal
			}
		}
	}
}

void co2_service_task_measure(void* pvParameters){
	
	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_CO2;
	
	for (;;)
	{
		_vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first 
		co2__service_measure((co2_service_t) pvParameters);
	}
}

co2_service_t co2_create(EventGroupHandle_t eventGroupMeassure, EventGroupHandle_t eventGroupDataReady){
	
	co2_service_t _service = malloc(sizeof(co2_measurement));
	if (NULL == _service){
		return NULL;
	}
	
	_service->ppm = 0;	
	
	_mutex = _xSemaphoreCreateMutex();
	
	_eventGroupMeasure = eventGroupMeassure;
	_eventGroupDataReady = eventGroupMeassure;
	
	_bitMeasureStart = DEF_BIT_MEASURE_START_CO2;
	_bitDataReady = DEF_BIT_DATA_READY_CO2;
	
	
	mh_z19_create(DEF_IO_PORT_CO2, NULL); 
	
	_xTaskCreate(
		co2_service_task_measure,		/* Function that implements the task. */
		"co2 task",					/* Text name for the task. */
		DEF_STACK_CO2,			/* Stack size in words, not bytes. */
		_service,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_CO2,	/* Priority at which the task is created. */
		NULL					/* Used to pass out the created task's handle. */
	);
	
	return _service;
}