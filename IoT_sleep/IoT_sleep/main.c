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
#include <display_7seg.h>
#include "co2.h"

// LoRaWAN
#include <lora_driver.h>
#include "uplinkHandler.h"	

// Global scope event and buffers
EventGroupHandle_t eventGroupMeasure = NULL;
EventGroupHandle_t eventGroupDataReady = NULL;


void create_operations(void){

	// create event groups
	eventGroupMeasure  = xEventGroupCreate();
	eventGroupDataReady = xEventGroupCreate();
	
	// create message buffers
	
	
	// create tasks
	co2_sensor_t co2Sensor = co2_create(eventGroupMeasure, eventGroupDataReady);
	
	
	// CO2 sensor passed to uplink handler is temporary.......
	// create LoRaWAN
	uplink_handler_create(co2Sensor);
	
}


/*-----------------------------------------------------------*/
void initialiseSystem()
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
	// Initialise the LoRaWAN driver without down-link buffer
	lora_driver_create(1, NULL);
	
	// Here the call back function is not needed
	display_7seg_init(NULL);

	// Power up the display
	display_7seg_powerUp();
}


/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!!
	
	// Create tasks
	create_operations();

	printf("Program Started!!\n");
	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}

