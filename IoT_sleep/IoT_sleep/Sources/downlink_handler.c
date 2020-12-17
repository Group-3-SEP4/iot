#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>

#include "downlink_handler.h"
#include "definitions.h"
#include "wrapper_messageBuffer.h"
#include "wrapper_task.h"
#include "configuration_service.h"
#include "secure_print.h"

#define CLASS_NAME	"downlinkHandler.c"

static configuration_service_t configuration;

static void downlink_handler_task( void *pvParameters )
{
	MessageBufferHandle_t buffer_downlink = pvParameters;
	
	for(;;)
	{
		lora_driver_payload_t _payload;
		size_t xReceivedBytes;
		
		xReceivedBytes = _xMessageBufferReceive( buffer_downlink,( void * ) &_payload, sizeof(lora_driver_payload_t ), DEF_WAIT_BUFFER_DOWNLINK);
		
		if(xReceivedBytes > 0)
		{
			s_print("PROD", CLASS_NAME, "Received new data.");
			configuration_service_set_temperature(configuration, (uint16_t)((_payload.bytes[0] << 8) + _payload.bytes[1]));
			configuration_service_set_min_co2(configuration, (uint16_t)((_payload.bytes[2] << 8) + _payload.bytes[3]));
			configuration_service_set_max_co2(configuration, (uint16_t)((_payload.bytes[4] << 8) + _payload.bytes[5]));
		} else {
			s_print("PROD", CLASS_NAME, "Buffer empty looping around.");
		}
	}
}

void downlink_handler_create(MessageBufferHandle_t buffer_downlink, configuration_service_t configuration_service)
{
	configuration = configuration_service;
	
	_xTaskCreate(
	downlink_handler_task
	,  "Down link handler"  // A name just for humans
	,  DEF_STACK_DOWNLINK  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  buffer_downlink
	,  DEF_PRIORITY_TASK_DOWNLINK  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}
