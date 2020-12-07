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
#include "secrets.h"
#include "sensorDataHandler.h"
#include "definitions.h"
#include "downlinkHandler.h"

// Globals
EventGroupHandle_t eventGroupMeasure = NULL;
EventGroupHandle_t eventGroupDataReady = NULL;
MessageBufferHandle_t uplinkMessageBuffer;
MessageBufferHandle_t messageBuffer;
configuration_t configuration;

// Locals
void initialize_hardware(void);
void initialize_lora(void);
void initialize_globals(void);
void start_tasks(void);

static char _out_buf[100];

int main(void)
{
	initialize_globals();
	
	initialize_hardware(); // Must be done as the very first thing!!
	
	start_tasks();

	printf("Program Started!!\n");
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}


void initialize_globals(void){
	// read configuration
	// initialize configuration
	configuration = configuration_create();
	
	// create event groups
	eventGroupMeasure  = xEventGroupCreate();
	eventGroupDataReady = xEventGroupCreate();
	
	// create message buffers
	uplinkMessageBuffer = xMessageBufferCreate( DEF_MESSAGE_BUFFER_UPLINK );
	if(NULL == uplinkMessageBuffer){
		printf("Not enough memory available for uplink message buffer!!\n");
	}
	messageBuffer = xMessageBufferCreate( DEF_MESSAGE_BUFFER_UPLINK );
}

void start_tasks(void){
	co2_sensor_t co2Sensor = co2_create(eventGroupMeasure, eventGroupDataReady);
	
	downlinkHandler_create(configuration, messageBuffer);
	
	uplink_handler_create(co2Sensor);
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
		
	// Initialize the LoRaWAN driver without down-link buffer
	lora_driver_create(1, messageBuffer);
	
	// Here the call back function is not needed
	display_7seg_init(NULL);

	// Power up the display
	display_7seg_powerUp();
	
	initialize_lora();
}

