/*
*  Author: Aron 264237
*/
#include <ATMEGA_FreeRTOS.h>
//#include <message_buffer.h>
#include <task.h>
#include <stdio.h>
//#include <event_groups.h>
#include "sensor_data_handler.h"

#include <wrapper_task.h>

#include "co2_service.h"
#include "ht_service.h"
#include "servo_service.h"
#include "payload_builder.h"
#include "definitions.h"
#include "FreeRTOSConfig.h"
#include "wrapper_eventGroup.h"
#include "wrapper_message_buffer.h"

#define CLASS_NAME "sensor_data_handler.c"

void sensor_data_handler_task(void *pvParameters);

static MessageBufferHandle_t _uplink_message_buffer;
static EventGroupHandle_t _event_group_data_collect;
static EventGroupHandle_t _event_group_data_ready;

static co2_t _co2_service;
static ht_t _ht_service;
static servo_t _servo_service;

void sensor_data_handler_create(
MessageBufferHandle_t messageBuffer
, EventGroupHandle_t event_group_data_collect
, EventGroupHandle_t event_group_data_ready
, co2_t co2_service
, ht_t ht_service
, servo_t servo_service){
	_uplink_message_buffer = messageBuffer;
	_event_group_data_collect = event_group_data_collect;
	_event_group_data_ready = event_group_data_ready;
	_co2_service = co2_service;
	_ht_service = ht_service;
	_servo_service = _servo_service;
	
	_xTaskCreate(
	sensor_data_handler_task		/* Function that implements the task. */
	, "sdh_task"		/* Text name for the task. */
	, DEF_STACK_SENSOR_DATA_HANDLER			/* Stack size in words, not bytes. */
	, NULL			/* Parameter passed into the task. */
	, DEF_PRIORITY_TASK_SENSOR_DATA_HANDLER	/* Priority at which the task is created. */
	, NULL					/* Used to pass out the created task's handle. */
	);
}

void sensor_data_handler_task_body(payload_builder_t payload_builder){
	lora_driver_payload_t _uplink_payload;
	
	_xEventGroupSetBits(_event_group_data_collect, DEF_BIT_DATA_COLLECT_ALL);
	
	EventBits_t uxBits = _xEventGroupWaitBits(
	_event_group_data_ready /* The event group being tested. */
	, DEF_BIT_DATA_READY_ALL /* The bits within the event group to wait for. */
	, pdTRUE /* DEF_BIT_DATA_READY_ALL should be cleared before returning. */
	, pdTRUE /* wait for all bits*/
	, portMAX_DELAY ); /* Wait a maximum of 100ms for either bit to be set. */
	if( ( uxBits & DEF_BIT_DATA_READY_ALL  ) == ( DEF_BIT_DATA_READY_ALL ) )
	{
		payload_builder_set_co2_ppm(payload_builder, co2_service_get_measurement(_co2_service));
		payload_builder_set_humidity(payload_builder, ht_service_get_humidity(_ht_service));
		payload_builder_set_temperature(payload_builder, ht_service_get_temperature(_ht_service));
		payload_builder_set_servo_position(payload_builder, servo_service_get_position(_servo_service));
		
		payload_builder_get_lora_payload(payload_builder, &_uplink_payload);
		
		size_t xBytesSent = _xMessageBufferSend(
		_uplink_message_buffer,
		( void * ) &_uplink_payload,
		sizeof( _uplink_payload ),
		0
		);
		if( xBytesSent != sizeof( _uplink_payload ) )
		{
			printf("Uplink message buffer full, cannot send payload of size %d\n", sizeof(_uplink_payload));
		}
	}
	//else
	//{
	///* time ran out, won't happen*/
	//}
}

void sensor_data_handler_task(void *pvParameters){
	payload_builder_t _payload_builder = payload_builder_create();
	if (NULL == _payload_builder) {
		printf("sensor_data_handler_task: Error creating payload_builder_t, could not allocate memory!");
	}

	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_FREQUENCY_UPLINK;
	for (;;)
	{
		_vTaskDelayUntil( &xLastWakeTime, xFrequency );
		
		sensor_data_handler_task_body(_payload_builder);
	}
}