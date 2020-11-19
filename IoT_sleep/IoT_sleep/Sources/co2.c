#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include "co2.h"
#include "definitions.h"

typedef struct co2_sensor {
	uint16_t value;
} co2_sensor;


SemaphoreHandle_t _co2_mutex;

static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;

static EventBits_t _bitMeasureStart;
static EventBits_t _bitDataReady;


uint16_t co2_getMeasurement(co2_sensor_t sensor){
	uint16_t _tmpValue = DEF_DEFAULT_NA_CO2;
	if (xSemaphoreTake (_co2_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE)
	{
		_tmpValue = sensor->value;
		xSemaphoreGive(_co2_mutex);
	}
	return _tmpValue;
}


void co2_task_measure(void* pvParameters){
	
	co2_sensor_t _sensor = pvParameters;
	
	for (;;)
	{
		
		mh_z19_returnCode_t _returnCode = mh_z19_takeMeassuring();
		vTaskDelay(DEF_DELAY_TASK_CO2); 
		
		if(_returnCode == MHZ19_OK) 
		{
			if (xSemaphoreTake (_co2_mutex, DEF_WAIT_MUTEX_CO2) == pdTRUE)
			{
				mh_z19_getCo2Ppm(&_sensor->value);
				xSemaphoreGive(_co2_mutex);
				
				if (xEventGroupGetBits(_eventGroupMeasure) & _bitMeasureStart) // checks eventMeasureStart bits
				{
					xEventGroupClearBits(_eventGroupMeasure, _bitMeasureStart); // clears eventMeasure bits
					xEventGroupSetBits(_eventGroupDataReady, _bitDataReady); // sets eventDataReady bits
				}
			}
			printf("Current ppm: %i\n", co2_getMeasurement(_sensor)); // only for visual verification in terminal
		}
	}
}


co2_sensor_t co2_create(EventGroupHandle_t eventGroupMeassure, EventGroupHandle_t eventGroupDataReady){
	
	co2_sensor_t _sensor = calloc(1, sizeof(co2_sensor));
	if (NULL == _sensor){
		return NULL;
	}
	
	_sensor->value = 0;	
	
	_co2_mutex = xSemaphoreCreateMutex();
	
	_eventGroupMeasure = eventGroupMeassure;
	_eventGroupDataReady = eventGroupMeassure;
	
	_bitMeasureStart = DEF_BIT_MEASURE_START_CO2;
	_bitDataReady = DEF_BIT_DATA_READY_CO2;
	
	
	mh_z19_create(DEF_IO_PORT_CO2, NULL); 
	
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






	





