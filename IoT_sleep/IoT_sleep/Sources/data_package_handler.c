#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <stddef.h>
#include <lora_driver.h>
#include "data_package_handler.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_messageBuffer.h"
#include "wrapper_task.h"
#include "co2_service.h"
#include "ht_service.h"
#include "servo_service.h"
#include "payload_builder.h"
#include "secure_print.h"

#define CLASS_NAME	"data_package_handler.c"

static EventGroupHandle_t event_collect;
static EventGroupHandle_t event_ready;
static MessageBufferHandle_t buffer_uplink;

static co2_service_t co2;
static ht_service_t ht;
static servo_service_t servo;


void data_package_handler_collect_data(){
	
	_xEventGroupSetBits(event_collect, DEF_BIT_DATA_COLLECT_ALL);
	
	EventBits_t data_bits = _xEventGroupWaitBits(
	event_ready,	// The event group being tested.
	DEF_BIT_DATA_READY_ALL,			// The bits to wait for.
	pdTRUE,					// bits will be cleared before return
	pdTRUE,					// wait for bits to be set
	DEF_WAIT_EVENT_DATA_READY);	// time to wait
	
	if ((data_bits & DEF_BIT_DATA_READY_ALL) == DEF_BIT_DATA_READY_ALL){
		
		int16_t tt = ht_service_get_temperature(ht);
		uint16_t co = co2_service_get_measurement(co2);
		uint16_t sPos = servo_service_get_position(servo);
		uint16_t rh = ht_service_get_humidity(ht);
		
		_xEventGroupClearBits(event_collect, DEF_BIT_DATA_READY_ALL);
	
		lora_driver_payload_t payload = payload_builder_get_payload(tt, rh, co, sPos);
		
		_xMessageBufferSend(buffer_uplink, &payload, sizeof(lora_driver_payload_t), DEF_WAIT_BUFFER_UPLINK_FULL);
	}
}


static void data_package_handler_task(void* pvParameters){	
	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_SEND_PAYLOAD;
	
	for (;;)
	{
		_vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		data_package_handler_collect_data();
	}
}


void data_package_handler_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready, MessageBufferHandle_t message_buffer_uplink, co2_service_t co2_service, ht_service_t ht_service, servo_service_t servo_service){
	
	co2 = co2_service;
	ht = ht_service;
	servo = servo_service;
	
	event_collect = event_group_data_collect;
	event_ready = event_group_data_ready;
	buffer_uplink = message_buffer_uplink;
	
	_xTaskCreate(
	data_package_handler_task,		/* Function that implements the task. */
	"data package task",			/* Text name for the task. */
	DEF_STACK_DATAPACKAGE,			/* Stack size in words, not bytes. */
	NULL,			/* Parameter passed into the task. */
	DEF_PRIORITY_TASK_DATAPACKAGE,	/* Priority at which the task is created. */
	NULL);							/* Used to pass out the created task's handle. */

}


