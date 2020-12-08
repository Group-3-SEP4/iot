#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

//#include <hal_defs.h>
#include <ihal.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <FreeRTOSTraceDriver.h>
#include <stdio_driver.h>
#include <serial.h>
#include <event_groups.h>
#include <message_buffer.h>
#include <display_7seg.h>
#include <status_leds.h>
#include <lora_driver.h>
#include "co2_sensor.h"
#include "configuration.h"
#include "uplinkHandler.h"	
#include "downlinkHandler.h"
#include "definitions.h"
#include "sensorDataHandler.h"

void initialize_hardware(void);
void start_tasks(void);

static MessageBufferHandle_t _downlinkMessageBuffer =NULL;


int main(void)
{
	initialize_hardware(); // Must be done as the very first thing!!
		
	start_tasks();

	printf("Program Started!!\n");
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}


void start_tasks(void){
// support structures
	configuration_t config = configuration_create();
	EventGroupHandle_t eventGroupMeasure  = xEventGroupCreate();
	EventGroupHandle_t eventGroupDataReady = xEventGroupCreate();

	MessageBufferHandle_t uplinkMessageBuffer = xMessageBufferCreate( 100 );
	if(NULL == uplinkMessageBuffer){
		printf("Not enough memory available for uplink message buffer!!\n");
	}
	
	
// tasks (will only start when this method exits)
	co2_sensor_t co2Sensor = co2_create(eventGroupMeasure, eventGroupDataReady);

	// if both sensor_data_handler_create or downlinkHandler_create are executed, 
	// lora_driver_setOtaaIdentity in uplink_handler returns UNKNOWN

	sensor_data_handler_create(uplinkMessageBuffer, co2Sensor);
	uplink_handler_create(uplinkMessageBuffer);
	downlinkHandler_create(config, _downlinkMessageBuffer);
}

/*-----------------------------------------------------------*/

void initialize_hardware(void)
{
	// Set output ports for LEDs used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);
	// Initialize the trace-driver to be used together with the R2R-Network
	trace_init();
	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_create(ser_USART0);
	
	// LoRaWAN initialization 
	// Initialize the HAL layer and use 5 for LED driver priority
	hal_create(5);
		
	_downlinkMessageBuffer = xMessageBufferCreate( 100 );
	if(NULL == _downlinkMessageBuffer){
		printf("Not enough memory available for downlink message buffer!!\n");
	}
	// Initialize the LoRaWAN driver without down-link buffer
	lora_driver_create(1, _downlinkMessageBuffer);
	
	// Here the call back function is not needed
	display_7seg_init(NULL);

	// Power up the display
	display_7seg_powerUp();
}

