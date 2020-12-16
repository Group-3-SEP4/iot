#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <ihal.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <FreeRTOSTraceDriver.h>
#include <stdio_driver.h>
#include <serial.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <display_7seg.h>
#include <lora_driver.h>
#include <rc_servo.h>

#include "definitions.h"
#include "co2_service.h"
#include "ht_service.h"
#include "data_package_handler.h"
#include "configuration_service.h"
#include "uplink_handler.h"
#include "downlink_handler.h"
#include "servo_service.h"
#include "secure_print.h"

#define CLASS_NAME		"main.c"


void create_operations(MessageBufferHandle_t buffer_downlink){
	
	configuration_service_t configuration_service = configuration_service_create();

	EventGroupHandle_t event_group_data_collect  = xEventGroupCreate();
	EventGroupHandle_t event_group_data_ready = xEventGroupCreate();
	
	MessageBufferHandle_t buffer_uplink = xMessageBufferCreate(DEF_MESSAGE_BUFFER_UPLINK);

	co2_service_t co2_service = co2_service_create(event_group_data_collect, event_group_data_ready);
	
	ht_service_t ht_service = ht_service_create(event_group_data_collect, event_group_data_ready);
	
	servo_service_t servo_service = servo_service_create(OUT_J14, event_group_data_collect, event_group_data_ready, configuration_service, co2_service, ht_service);
	
	data_package_handler_create(event_group_data_collect, event_group_data_ready, buffer_uplink, co2_service, ht_service, servo_service);
	
	uplink_handler_create(buffer_uplink);
	
	downlink_handler_create(buffer_downlink, configuration_service);
}


void initialiseSystem(MessageBufferHandle_t buffer_downlink)
{
	
	// Set output ports for leds used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);
	// Initialise the trace-driver to be used together with the R2R-Network
	trace_init();
	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_create(ser_USART0);
	
	// LoRaWAN initialization
	// Initialise the HAL layer and use 5 for LED driver priority
	hal_create(5);
	
	// initialize the servo
	rc_servo_create();

	// Initialise the LoRaWAN
	lora_driver_create(LORA_USART, buffer_downlink);
	
	// Here the call back function is not needed
	display_7seg_init(NULL);

	// Power up the display
	display_7seg_powerUp();
}


int main(void)
{
	MessageBufferHandle_t buffer_downlink = xMessageBufferCreate(DEF_MESSAGE_BUFFER_DOWNLINK);
	s_print_create(xSemaphoreCreateMutex()); // initialize s_print
	
	initialiseSystem(buffer_downlink); // Must be done as the very first thing!!
	
	// Create tasks
	create_operations(buffer_downlink);

	s_print("PROD", CLASS_NAME, "Program Started!!");

	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}

