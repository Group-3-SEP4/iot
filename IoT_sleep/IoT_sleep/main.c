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
#include "co2_sensor.h"
#include "ht_sensor.h"
#include "dataPackageHandler.h"
#include "configuration.h"
#include "uplinkHandler.h"
#include "downlinkHandler.h"
#include "servo.h"
#include "secure_print.h"

#define CLASS_NAME		"main.c"


void create_operations(MessageBufferHandle_t msgBufferDownlink){
	
	configuration_t configuration = configuration_create();

	EventGroupHandle_t eventGroupMeasure  = xEventGroupCreate();
	EventGroupHandle_t eventGroupDataReady = xEventGroupCreate();
	
	MessageBufferHandle_t msgBufferUplink = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);

	co2_sensor_t co2Sensor = co2_create(eventGroupMeasure, eventGroupDataReady);
	
	ht_sensor_t htSensor = ht_create(eventGroupMeasure, eventGroupDataReady);
	
	servo_t servo = servo_create(SERVO_J14, eventGroupMeasure, eventGroupDataReady, configuration, co2Sensor, htSensor);
	
	dataPackageHandler_create(eventGroupMeasure, eventGroupDataReady, msgBufferUplink, co2Sensor, htSensor, servo);
	
	uplink_handler_create(msgBufferUplink);
	downlink_handler_create(msgBufferDownlink, configuration);
}


void initialiseSystem(MessageBufferHandle_t msgBufferDownlink)
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
	lora_driver_create(LORA_USART, msgBufferDownlink);
	
	// Here the call back function is not needed
	display_7seg_init(NULL);

	// Power up the display
	display_7seg_powerUp();
}


int main(void)
{
	MessageBufferHandle_t msgBufferDownlink = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
	s_print_create(xSemaphoreCreateMutex()); // initialize s_print
	
	initialiseSystem(msgBufferDownlink); // Must be done as the very first thing!!
	
	// Create tasks
	create_operations(msgBufferDownlink);

	s_print("INFO", CLASS_NAME, "Program Started!! Free heap: %i", xPortGetFreeHeapSize() );

	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}

