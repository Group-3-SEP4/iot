
/*
 * co2_task.c
 *
 * Created: 25/11/2020 10.30.44
 *  Author: Jacob
 */ 

#include <ATMEGA_FreeRTOS.h>
#include "co2_sensor.h"
#include "definitions.h"


static void co2_task_measure(void* pvParameters){
	
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_CO2;
	
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		co2_measure((co2_sensor_t) pvParameters);
	}
}

void co2_task_create(co2_sensor_t sensor){
		xTaskCreate(
		co2_task_measure,		/* Function that implements the task. */
		"CO2 Sensor task",		/* Text name for the task. */
		DEF_STACK_CO2,			/* Stack size in words, not bytes. */
		sensor,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_CO2,	/* Priority at which the task is created. */
		NULL					/* Used to pass out the created task's handle. */
		);
}