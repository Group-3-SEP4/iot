#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <stddef.h>
#include <semphr.h>
#include <lora_driver.h>
#include "dataPackageHandler.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_messageBuffer.h"
#include "co2_sensor.h"
#include "ht_sensor.h"
#include "servo.h"
#include "payloadBuilder.h"
#include "secure_print.h"

#define CLASS_NAME	"dataPackageHandler.c"

static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;
static MessageBufferHandle_t _msgBufferUplink;

static co2_sensor_t _co2_sensor;
static ht_sensor_t _ht_sensor;
static servo_t _servo;


void dataPackageHandler_collectSensorData(){
	
	_xEventGroupSetBits(_eventGroupMeasure, DEF_BIT_MEASURE_START);
	
	EventBits_t dataBits = xEventGroupWaitBits(
	_eventGroupDataReady,	// The event group being tested.
	DEF_BIT_DATA_READY,			// The bits to wait for.
	pdTRUE,					// bits will be cleared before return
	pdTRUE,					// wait for bits to be set
	DEF_WAIT_EVENT_DATA_READY);	// time to wait
	
	
	if ((dataBits & DEF_BIT_DATA_READY) == DEF_BIT_DATA_READY){
		
		int16_t tt = ht_getTemperature(_ht_sensor);
		uint16_t co = co2_getMeasurement(_co2_sensor);
		uint16_t sPos = servo_getPosition(_servo);
		uint16_t rh = ht_getHumidity(_ht_sensor);
		
		_xEventGroupClearBits(_eventGroupMeasure, DEF_BIT_DATA_READY);
	
		lora_driver_payload_t payload = payloadBuilder_getPayload(tt, rh, co, sPos);
		
		s_print("INFO", CLASS_NAME, "payload: TT %i, RH %i CO2 %i, sPos %i", tt, rh, co, sPos);
		
		_xMessageBufferSend(_msgBufferUplink, &payload, sizeof(lora_driver_payload_t), DEF_WAIT_MSG_BUFFER_FULL_DATAPACKGE);
	}
}


void dataPackageHandler_task(void* pvParameters){	
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_SEND_DATAPACKAGE;
	
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		dataPackageHandler_collectSensorData();
	}
}


void dataPackageHandler_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, MessageBufferHandle_t messageBufferuplink, co2_sensor_t co2_sensor, ht_sensor_t ht_sensor, servo_t servo){
	
	_co2_sensor = co2_sensor;
	_ht_sensor = ht_sensor;
	_servo = servo;
	
	_eventGroupMeasure = eventGroupMeasure;
	_eventGroupDataReady = eventGroupDataReady;
	_msgBufferUplink = messageBufferuplink;
	
	xTaskCreate(
	dataPackageHandler_task,		/* Function that implements the task. */
	"data package task",			/* Text name for the task. */
	DEF_STACK_DATAPACKAGE,			/* Stack size in words, not bytes. */
	NULL,			/* Parameter passed into the task. */
	DEF_PRIORITY_TASK_DATAPACKAGE,	/* Priority at which the task is created. */
	NULL);							/* Used to pass out the created task's handle. */

}


