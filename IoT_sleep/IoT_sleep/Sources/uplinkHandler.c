#include <stddef.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <display_7seg.h>
#include "definitions.h"
#include "payloadBuilder.h"

static bool initialized = false;

void uplink_handler_task( void *pvParameters );

static MessageBufferHandle_t _uplinkMessageBuffer;

void uplink_handler_create(MessageBufferHandle_t messageBuffer)
{
	_uplinkMessageBuffer = messageBuffer;
	
	xTaskCreate(
	   uplink_handler_task
	,  (const portCHAR *)"UplinkHandler"  // A name just for humans
	,  DEF_STACK_UPLINK  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  DEF_PRIORITY_TASK_UPLINK  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}

void uplink_handler_task( void *pvParameters )
{
	if (!initialized){
		//TODO: figure out if this can be moved to initialization method in main
		// Hardware reset of LoRaWAN transceiver
		lora_driver_resetRn2483(1);
		vTaskDelay(2);
		lora_driver_resetRn2483(0);
		// Give it a chance to wakeup
		vTaskDelay(150);

		lora_driver_flushBuffers(); // get rid of first version string from module after reset!
		
		initialized = true;
	}
	
	float packagesSent = 0.0;
	 
	for(;;) // task might need to loop else where, so it can be called by buffer
	{
		lora_driver_payload_t uplink_payload;
		size_t xReceivedBytes = xMessageBufferReceive( _uplinkMessageBuffer,
		( void * ) &uplink_payload,
		sizeof( uplink_payload ),
		portMAX_DELAY );
		if( xReceivedBytes > 0 )
		{
			display_7seg_display(++packagesSent, 0);

			status_leds_shortPuls(led_ST4);  // OPTIONAL
			
			char * returnCode = lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &uplink_payload));
			uint16_t co2Value = (uplink_payload.bytes[4] << 8) | uplink_payload.bytes[5]; 
			
			printf("Upload Message: CO2 value: %i ppm - >%s<\n", co2Value, returnCode);
		}
		else {
			printf("uplinkHandler encountered an error reading from the uplinkMessageBuffer");
		}
		
	}
}
