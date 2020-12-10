/*
*  Author: Aron 264237
*/

#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>
#include <task.h>
#include <stdio.h>
#include "sensor_data_handler.h"
#include "co2_service.h"
#include "payload_builder.h"
#include "definitions.h"
#include "FreeRTOSConfig.h"

void sensor_data_handler_task(void *pvParameters);

co2_t _co2_service;
MessageBufferHandle_t _uplink_message_buffer;
lora_driver_payload_t _uplink_payload;
payload_builder_t _payload_builder;

void sensor_data_handler_create(MessageBufferHandle_t messageBuffer, co2_t co2_service){
	_uplink_message_buffer = messageBuffer;
	_co2_service = co2_service;
	
	_payload_builder = payload_builder_create();
	
	xTaskCreate(
		sensor_data_handler_task,		/* Function that implements the task. */
		"sdh_task",		/* Text name for the task. */
		DEF_STACK_SENSOR_DATA_HANDLER,			/* Stack size in words, not bytes. */
		NULL,				/* Parameter passed into the task. */
		DEF_PRIORITY_TASK_SENSOR_DATA_HANDLER,	/* Priority at which the task is created. */
		NULL					/* Used to pass out the created task's handle. */
	);
}

void sensor_data_handler_task(void *pvParameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_FREQUENCY_UPLINK;
	
	for (;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		// TODO: get the value with smphr or mutex
		uint16_t co2_ppm = co2_service_get_measurement(_co2_service);
		
		payload_builder_set_co2_ppm(_payload_builder,co2_ppm);
		
		payload_builder_get_lora_payload(_payload_builder, &_uplink_payload);
		
		//printf("payload size: %d\n", sizeof(_uplink_payload));
		
		size_t xBytesSent = xMessageBufferSend(
			_uplink_message_buffer,
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