#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include "co2.h"
#include "definitions.h"

typedef struct co2_sensor {
	uint16_t value;
	TaskHandle_t taskHandle;
} co2_sensor;


// tasks
void co2_task_meassure(void* pvParameters);


static EventGroupHandle_t _eventGroupMeassure;
static EventGroupHandle_t _eventGroupDataReady;

static EventBits_t _bitMeassureStart;
static EventBits_t _bitDataReady;


co2_sensor_t co2_create(EventGroupHandle_t eventGroupMeassure, EventGroupHandle_t eventGroupDataReady){
	
	co2_sensor_t _sensor = calloc(1, sizeof(co2_sensor));
	if (NULL == _sensor){
		return NULL;
	}
	
	_sensor->value = 0;
	
	_eventGroupMeassure = eventGroupMeassure;
	_eventGroupDataReady = eventGroupMeassure;
	
	_bitMeassureStart = DEF_BIT_MEASSURE_START_CO2;
	_bitDataReady = DEF_BIT_DATA_READY_CO2;
	
	
	mh_z19_create(DEF_IO_PORT_CO2, NULL); 
	
	xTaskCreate(
		co2_task_meassure,		/* Function that implements the task. */
		"CO2 Sensor task",		/* Text name for the task. */
		DEF_STACK_CO2 + 200,	/* Stack size in words, not bytes. */
		_sensor,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_CO2,	/* Priority at which the task is created. */
		&_sensor->taskHandle	/* Used to pass out the created task's handle. */
	);
	
	return _sensor;
}


void co2_destroy(co2_sensor_t* sensor){
	if (sensor == NULL) {
		return;
	}
	
	co2_sensor_t _sensor = *sensor;
	vTaskDelete(_sensor->taskHandle);
	vPortFree(_sensor);
}



void co2_task_meassure(void* pvParameters){
	
	// Remove compiler warnings 
	co2_sensor_t _sensor = pvParameters;
	
	for (;;){
		
		
		mh_z19_returnCode_t returnCode = mh_z19_takeMeassuring();
		
		vTaskDelay(DEF_DELAY_TASK_CO2);
		if(returnCode == MHZ19_OK) {
			mh_z19_getCo2Ppm(&_sensor->value);
			printf("Current ppm: %i\n", _sensor->value);
		}
	}
}


// content might have to be moved.....
void co2_event_meassure(){

	EventBits_t bits = xEventGroupWaitBits(
	_eventGroupMeassure,	/* the event group being tested */
	_bitMeassureStart,		/* the bit to wait for */
	pdTRUE,					/* bits will be cleared before return */
	pdTRUE,					/* wait for bits to be set */
	portMAX_DELAY);			/* maximum time to wait */


	if ((bits & (_bitMeassureStart)) == (_bitMeassureStart)) { /* check bits */
	
		//handle start measure here
	
	
		xEventGroupSetBits(_eventGroupDataReady, _bitDataReady); /* measurement ready */
	
	
	}
}

	
uint16_t co2_getMeassure(co2_sensor_t sensor){
	return sensor->value;
}

	





