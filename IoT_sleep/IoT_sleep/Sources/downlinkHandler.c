#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>
#include <lora_driver.h>

#include "downlinkHandler.h"
#include "configuration.h"
#include "definitions.h"


static configuration_t _configuration;
static MessageBufferHandle_t _messageBuffer;

void downlinkHandler_task(void* pvParameters) {
	//printf("downlinkHandler_task: starting task\n");
	// while loop
	
	//MessageBufferHandle_t xMessageBuffer = pvParameters;
	/* Receive the next message from the message buffer.  Wait in the Blocked
    state (so not using any CPU processing time) for a maximum of 100ms for
    a message to become available. */
		vTaskDelay(1000); 
	
//	uint8_t ucRxData[ 20 ];
	for(;;) {
		
		lora_driver_payload_t _payload;
		uint8_t xReceivedBytes;
		
		//printf("downlinkHandler_task: waiting for messages\n");
		xReceivedBytes = xMessageBufferReceive(_messageBuffer, (void*) &_payload, sizeof(lora_driver_payload_t), portMAX_DELAY); // wait forever
		
		vTaskDelay(150);
		if( xReceivedBytes > 0 )
		{
			uint16_t tempSetpoint;
			uint16_t co2Min;
			uint16_t co2Max;
			
			switch (_payload.port_no)
			{
				case 2:
				tempSetpoint = (_payload.bytes[0] << 8) + _payload.bytes[1];
				co2Min = (_payload.bytes[2] << 8) + _payload.bytes[3];
				co2Max = (_payload.bytes[4] << 8) + _payload.bytes[5];
				
				//if (DEF_PRINT_TO_TERMINAL){
					//printf("Received TT: %i, co2 min: %i, co2 max: %i \n", tempSetpoint, co2Min, co2Max);
				//}
				//
				configuration_setDefaultTemperatur(_configuration, tempSetpoint);
				configuration_setMinCo2(_configuration, co2Min);
				configuration_setMaxCo2(_configuration, co2Max);
			}
		} else {
			if (DEF_PRINT_TO_TERMINAL){
				printf("Buffer empty looping around \n");
			}
		}
		
	}
}

void downlinkHandler_create(configuration_t configuration, MessageBufferHandle_t messageBuffer) {
	
	_configuration = configuration;
	_messageBuffer = messageBuffer;
	
	xTaskCreate(
	downlinkHandler_task,
	(const portCHAR *)"DownlinkHandler",
	DEF_STACK_DOWNLINK,
	NULL,
	DEF_PRIORITY_TASK_LINK,
	NULL
	);
}

