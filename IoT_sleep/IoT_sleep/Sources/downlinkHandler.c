#include <stdlib.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <stddef.h>
#include <message_buffer.h>
#include <lora_driver.h>


#include "downlinkHandler.h"
#include "configuration.h"
#include "definitions.h"


static configuration_t _configuration;

void downlinkHandler_task(void* xMessageBuffer) {
	printf("downlinkHandler_task: starting task\n");
	// while loop
	
	/* Receive the next message from the message buffer.  Wait in the Blocked
    state (so not using any CPU processing time) for a maximum of 100ms for
    a message to become available. */
	
//	uint8_t ucRxData[ 20 ];
	for(;;) {
		vTaskDelay(50); 
		
		lora_driver_payload_t downlinkPayload;
		uint8_t xReceivedBytes;
		
		printf("downlinkHandler_task: waiting for messages\n");
		xReceivedBytes = xMessageBufferReceive(xMessageBuffer, (void*) &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY); // wait forever
		
		

		if(xReceivedBytes > 0)
		{
			printf("downlinkHandler_task: ------------------- Receive Data\n");
			// TODO: perhaps eventgroup to tell that we received updated conf? This could also be done in the configuration itself
			//0012032003E8 // 18 800 1000
			
			configuration_setDefaultTemperatur(_configuration, (uint16_t)((downlinkPayload.bytes[0] << 8) + downlinkPayload.bytes[1]));
			configuration_setMinCo2(_configuration, (uint16_t)((downlinkPayload.bytes[2] << 8) + downlinkPayload.bytes[3]));
			configuration_setMaxCo2(_configuration, (uint16_t)((downlinkPayload.bytes[4] << 8) + downlinkPayload.bytes[5]));
			printf("downlinkHandler_task: Received the following values[ temp: %d, Min Co2: %d, Max Co2: %d]\n", configuration_getDefaultTemperatur(_configuration), configuration_getMinCo2(_configuration), configuration_getMaxCo2(_configuration));
		} else {
			printf("downlinkHandler_task: Received zero data\n");
		}
		
	}
}

void downlinkHandler_create(configuration_t configuration, MessageBufferHandle_t messageBuffer) {
	
	_configuration = configuration;
	
	
	xTaskCreate(
	downlinkHandler_task,
	(const portCHAR *)"DownlinkHandler",
	DEF_STACK_DOWNLINK,
	messageBuffer,
	DEF_PRIORITY_TASK_LINK,
	NULL
	);
}

