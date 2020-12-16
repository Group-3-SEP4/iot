#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include "co2_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_task.h"
#include "secure_print.h"

#define CLASS_NAME	"co2_service.c"

typedef struct co2_service {
	uint16_t ppm;
	SemaphoreHandle_t mutex;
	EventGroupHandle_t event_collect;
	EventGroupHandle_t event_ready;
} co2_service_st; 


uint16_t co2_service_get_measurement(co2_service_t service){
	uint16_t tmp_value = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE)
	{
		tmp_value = service->ppm;
		_xSemaphoreGive(service->mutex);
	}
	return tmp_value;
}


inline void co2_service_measure(co2_service_t service){
	
	mh_z19_returnCode_t return_code = mh_z19_takeMeassuring();
	_vTaskDelay(DEF_DELAY_DRIVER_CO2); // delay must be placed here between takeMessuring() and check, since it takes time for the driver to measure (Async). If moved/deleted value will be 0 ppm.
	if(return_code == MHZ19_OK)
	{
		if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE) // protect shared data
		{
			mh_z19_getCo2Ppm(&service->ppm);
			_xSemaphoreGive(service->mutex);
			
			if (_xEventGroupGetBits(service->event_collect) & DEF_BIT_DATA_COLLECT_CO2) // checks eventMeasureStart bits
			{	
				_xEventGroupClearBits(service->event_collect, DEF_BIT_DATA_COLLECT_CO2); // clears eventMeasure bits
				_xEventGroupSetBits(service->event_ready, DEF_BIT_DATA_READY_CO2); // sets eventDataReady bits
			}
			s_print("INFO", CLASS_NAME, "Current CO2: %i", co2_service_get_measurement(service));
		}
	}
}


static void co2_service_task(void* pvParameters){
	
	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_CO2;
	
	for (;;)
	{
		_vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first 
		co2_service_measure((co2_service_t) pvParameters);
	}
}


co2_service_t co2_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready){
	
	co2_service_t service = malloc(sizeof(co2_service_st));
	if (NULL == service){
		return NULL;
	}
	
	service->ppm = DEF_DEFAULT_NA_SENSOR;	
	
	service->mutex = _xSemaphoreCreateMutex();
	
	service->event_collect = event_group_data_collect;
	service->event_ready = event_group_data_ready;
	
	mh_z19_create(MH_Z19_USART, NULL); 
	
	_xTaskCreate(
		co2_service_task,		/* Function that implements the task. */
		"CO2 task",			/* Text name for the task. */
		DEF_STACK_CO2,			/* Stack size in words, not bytes. */
		service,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_CO2,	/* Priority at which the task is created. */
		NULL					/* Used to pass out the created task's handle. */
	);
	
	return service;
}