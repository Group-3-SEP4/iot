#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <stddef.h>
#include <semphr.h>
#include "dataPackageHandler.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_messageBuffer.h"
#include "co2_sensor.h"

#include <lora_driver.h>


typedef struct dataPackage {
	int16_t temperature;
	int16_t humidity;
	uint16_t co2;
} dataPackage;


static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;
static MessageBufferHandle_t _msgBufferUplink;

static EventBits_t _bitMeasureStart;
static EventBits_t _bitDataReady;

static co2_sensor_t _co2_sensor;
static SemaphoreHandle_t _data_mutex;

static lora_driver_payload_t _uplink_payload;

lora_driver_payload_t dataPackageHandler_getPayload(dataPackageHandler_t dataPackageHandler){
	uint16_t hum = DEF_DEFAULT_NA_SENSOR; 
	int16_t temp = DEF_DEFAULT_NA_SENSOR; 
	uint16_t co2_ppm = DEF_DEFAULT_NA_SENSOR;
	uint16_t servo = DEF_DEFAULT_NA_SENSOR;
	
	if (_xSemaphoreTake (_data_mutex, DEF_WAIT_MUTEX_DATAPACKAGE) == pdTRUE)
	{
		// add more when implemented
		co2_ppm = dataPackageHandler->co2;
	
		_xSemaphoreGive(_data_mutex);
	}
	
	_uplink_payload.bytes[0] = hum >> 8; 
	_uplink_payload.bytes[1] = hum & 0xFF;
	_uplink_payload.bytes[2] = temp >> 8;
	_uplink_payload.bytes[3] = temp & 0xFF;
	_uplink_payload.bytes[4] = co2_ppm >> 8;
	_uplink_payload.bytes[5] = co2_ppm & 0xFF;
	_uplink_payload.bytes[6] = servo >> 8;
	_uplink_payload.bytes[7] = servo & 0xFF;

	if (DEF_PRINT_TO_TERMINAL){
		printf("Payload data: CO2: %i ppm \n", co2_ppm);
	}

	return _uplink_payload;
}
	

inline static void dataPackageHandler_collectSensorData(dataPackageHandler_t dataPackage){
	
	_xEventGroupSetBits(_eventGroupMeasure, _bitMeasureStart);
	
	EventBits_t dataBits = _xEventGroupWaitBits(
	_eventGroupDataReady,	// The event group being tested.
	_bitDataReady,			// The bits to wait for.
	pdTRUE,					// bits will be cleared before return
	pdTRUE,					// wait for bits to be set
	DEF_WAIT_EVENT_DATA_READY);	// time to wait
	
	if ((dataBits & _bitDataReady) == _bitDataReady){
		if (_xSemaphoreTake (_data_mutex, DEF_WAIT_MUTEX_DATAPACKAGE) == pdTRUE) // protect shared data
		{
			dataPackage->co2 = co2_getMeasurement(_co2_sensor);
			
			_xSemaphoreGive(_data_mutex);
			_xEventGroupClearBits(_eventGroupMeasure, _bitDataReady);
			
			lora_driver_payload_t payload = dataPackageHandler_getPayload(dataPackage);
			
			_xMessageBufferSend(_msgBufferUplink, &payload, sizeof(lora_driver_payload_t), DEF_WAIT_MSG_BUFFER_SEND_DATAPACKGE);
		}
	}
}


void dataPackageHandler_task(void* pvParameters){	
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_SEND_DATAPACKAGE;
	
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		dataPackageHandler_collectSensorData((dataPackageHandler_t) pvParameters);
	}
}


dataPackageHandler_t dataPackageHandler_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, MessageBufferHandle_t messageBufferuplink, co2_sensor_t co2_sensor){
	
	dataPackageHandler_t _dataPackageHandler = malloc(sizeof(dataPackage));
	if (NULL == _dataPackageHandler){
		return NULL;
	}
	
	_dataPackageHandler->temperature = DEF_DEFAULT_NA_SENSOR;
	_dataPackageHandler->humidity = DEF_DEFAULT_NA_SENSOR;
	_dataPackageHandler->co2 = DEF_DEFAULT_NA_SENSOR;
	
	_co2_sensor = co2_sensor;
	
	_data_mutex = xSemaphoreCreateMutex();
	
	_eventGroupMeasure = eventGroupMeasure;
	_eventGroupDataReady = eventGroupDataReady;
	_msgBufferUplink = messageBufferuplink;
	
	_bitMeasureStart = DEF_BIT_MEASURE_START;
	_bitDataReady = DEF_BIT_DATA_READY;
	
	_uplink_payload.len = 8;
	_uplink_payload.port_no = 2;

	xTaskCreate(
	dataPackageHandler_task,		/* Function that implements the task. */
	"data package task",			/* Text name for the task. */
	DEF_STACK_DATAPACKAGE,			/* Stack size in words, not bytes. */
	_dataPackageHandler,			/* Parameter passed into the task. */
	DEF_PRIORITY_TASK_DATAPACKAGE,	/* Priority at which the task is created. */
	NULL);							/* Used to pass out the created task's handle. */
	
	
	return _dataPackageHandler;
	
}


