/*
*	Author: Jesper (273961)
*/
#include <stdlib.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <stddef.h>
#include <message_buffer.h>
#include <lora_driver.h>
#include "secure_print.h"
#include "downlink_handler.h"
#include "configuration_service.h"
#include "definitions.h"

#define CLASS_NAME	"downlink_handler.c"

configuration_t _configuration;

void downlink_handler(void* message_buffer) {
	
		lora_driver_payload_t downlink_payload;
		uint8_t received_bytes;
		
		received_bytes = xMessageBufferReceive(message_buffer, (void*) &downlink_payload, sizeof(lora_driver_payload_t), portMAX_DELAY); // wait forever
		
		if(received_bytes > 0)
		{
			configuration_service_set_temperature(_configuration, (uint16_t)((downlink_payload.bytes[0] << 8) + downlink_payload.bytes[1]));
			configuration_service_set_min_co2(_configuration, (uint16_t)((downlink_payload.bytes[2] << 8) + downlink_payload.bytes[3]));
			configuration_service_set_max_co2(_configuration, (uint16_t)((downlink_payload.bytes[4] << 8) + downlink_payload.bytes[5]));
			s_print("INFO", CLASS_NAME, "Received the following values[ temp: %d, Min Co2: %d, Max Co2: %d]", configuration_service_get_temperature(_configuration), configuration_service_get_min_co2(_configuration), configuration_service_get_max_co2(_configuration));
		} else {
			s_print("WARNING", CLASS_NAME, "Received zero data");
		}
		
	
}

void downlink_handler_task(void* message_buffer){
	s_print("INFO", CLASS_NAME, "starting task");
	
	for(;;) {
		vTaskDelay(50);
		downlink_handler(message_buffer);
	}
}

void downlink_handler_create(configuration_t configuration, MessageBufferHandle_t message_buffer) {
	
	_configuration = configuration;
	
	
	xTaskCreate(
	downlink_handler_task,
	"downlink_handler",
	DEF_STACK_DOWNLINK,
	message_buffer,
	DEF_PRIORITY_TASK_LINK,
	NULL
	);
}

