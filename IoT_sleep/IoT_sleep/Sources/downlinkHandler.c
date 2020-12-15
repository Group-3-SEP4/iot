#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>
#include <lora_driver.h>

#include "downlinkHandler.h"
#include "definitions.h"
#include "wrapper_messageBuffer.h"
#include "configuration.h"

static configuration_t _configuration;

void downlink_handler_task( void *pvParameters )
{
	MessageBufferHandle_t _msgBufferDownlink = pvParameters;
	
	for(;;)
	{	
		lora_driver_payload_t _payload;
		size_t xReceivedBytes;
		
		xReceivedBytes = _xMessageBufferReceive( _msgBufferDownlink,( void * ) &_payload, sizeof(lora_driver_payload_t ), DEF_WAIT_MSG_BUFFER_DOWNLINK);
		
		if(xReceivedBytes > 0)
		{
			configuration_setDefaultTemperatur(_configuration, (uint16_t)((_payload.bytes[0] << 8) + _payload.bytes[1]));
			configuration_setMinCo2(_configuration, (uint16_t)((_payload.bytes[2] << 8) + _payload.bytes[3]));
			configuration_setMaxCo2(_configuration, (uint16_t)((_payload.bytes[4] << 8) + _payload.bytes[5]));
			if (DEF_PRINT_TO_TERMINAL){
					printf("Received data\n");
				}
		} else {
			if (DEF_PRINT_TO_TERMINAL){
				printf("Buffer empty looping around \n");
			}
		}
	}
}

void downlink_handler_create(MessageBufferHandle_t messageBufferDownlink, configuration_t configuration)
{	
	_configuration = configuration;
	
	xTaskCreate(
	downlink_handler_task
	,  (const portCHAR *)"Down link handler"  // A name just for humans
	,  DEF_STACK_DOWNLINK  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  messageBufferDownlink
	,  DEF_PRIORITY_TASK_DOWNLINK  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}