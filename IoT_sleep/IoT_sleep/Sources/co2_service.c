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
#include "secure_print.h"
#include "co2_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_task.h"

#define CLASS_NAME		"co2_service.c"

typedef struct co2_measurement {
	uint16_t ppm;
} co2_measurement;

static SemaphoreHandle_t _mutex;

static EventGroupHandle_t _event_group_data_collect;
static EventGroupHandle_t _event_group_data_ready;


uint16_t co2_service_get_measurement(co2_t sensor){
	uint16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE)
	{
		tmp_value = sensor->ppm;
		_xSemaphoreGive(_mutex);
	}
	return tmp_value;
}


inline void co2_service_measure(co2_t sensor){
	
	mh_z19_returnCode_t _return_code = mh_z19_takeMeassuring();
	_vTaskDelay(DEF_DELAY_DRIVER_CO2); // delay must be placed here between takeMessuring() and check, since it takes time for the driver to measure (Async). If moved/deleted value will be 0 ppm.
	if(_return_code == MHZ19_OK)
	{
		if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE) // protect shared data
		{
			mh_z19_getCo2Ppm(&sensor->ppm);
			_xSemaphoreGive(_mutex);
			
			if (_xEventGroupGetBits(_event_group_data_collect) & DEF_BIT_DATA_COLLECT_CO2) // checks eventMeasureStart bits
			{
				_xEventGroupClearBits(_event_group_data_collect, DEF_BIT_DATA_COLLECT_CO2); // clears eventMeasure bits
				_xEventGroupSetBits(_event_group_data_ready, DEF_BIT_DATA_READY_CO2); // sets eventDataReady bits
			}
			
			s_print("INFO", CLASS_NAME, "Current ppm: %i", co2_service_get_measurement(sensor)); // only for visual verification in terminal
		}
	}
}

void co2_service_task_measure(void* pvParameters){
	
	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_CO2;
	
	for (;;)
	{
		_vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first 
		co2_service_measure((co2_t) pvParameters);
	}
}

co2_t co2_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready){
	
	co2_t _service = malloc(sizeof(*_service));
	if (NULL == _service){
		return NULL;
	}
	
	_service->ppm = 0;	
	
	_mutex = _xSemaphoreCreateMutex();
	
	_event_group_data_collect = event_group_data_collect;
	_event_group_data_ready = event_group_data_ready;
	
	mh_z19_create(MH_Z19_USART, NULL); 
	
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