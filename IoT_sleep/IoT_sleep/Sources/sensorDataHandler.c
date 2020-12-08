/*
* sensorDataHandler.c
*
* Created: 04/12/2020 21.12.28
*  Author: agk13
*/
#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>
#include <task.h>
#include <stdio.h>
#include "sensorDataHandler.h"
#include "co2_sensor.h"
#include "payloadBuilder.h"
#include "definitions.h"
#include "FreeRTOSConfig.h"

void sensorDataHandler_task(void *pvParameters);

static co2_sensor_t _co2Sensor;
static MessageBufferHandle_t _uplinkMessageBuffer;
static lora_driver_payload_t _uplink_payload;
static payload_builder_t _payloadBuilder;

void sensor_data_handler_create(MessageBufferHandle_t messageBuffer, co2_sensor_t co2Sensor){
	_uplinkMessageBuffer = messageBuffer;
	_co2Sensor = co2Sensor;
	
	_payloadBuilder = payload_builder_create();
	
	xTaskCreate(
		sensorDataHandler_task,		/* Function that implements the task. */
		"Sensor data handler task",		/* Text name for the task. */
		DEF_STACK_SENSOR_DATA_HANDLER,			/* Stack size in words, not bytes. */
		NULL,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_SENSOR_DATA_HANDLER,	/* Priority at which the task is created. */
		NULL					/* Used to pass out the created task's handle. */
	);
}

void sensorDataHandler_task(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_FREQUENCY_UPLINK;
	vTaskDelay(100);
	for (;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		// TODO: get the value with smphr or mutex
		uint16_t co2_ppm = co2_getMeasurement(_co2Sensor);
		
		payload_builder_setCo2_ppm(_payloadBuilder,co2_ppm);
		
		payload_builder_getLoRaPayload(_payloadBuilder, &_uplink_payload);
		
		//printf("payload size: %d\n", sizeof(_uplink_payload));
		
		size_t xBytesSent = xMessageBufferSend(
			_uplinkMessageBuffer,
			( void * ) &_uplink_payload,
			sizeof( _uplink_payload ),
			0
		);// TODO: standardize the wait time
		if( xBytesSent != sizeof( _uplink_payload ) )
		{
			printf("Uplink message buffer full, cannot send payload of size %d\n", sizeof(_uplink_payload));
		}
	}
}