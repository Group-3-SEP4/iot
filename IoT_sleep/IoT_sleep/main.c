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

#include <display_7seg.h>

// LoRaWAN
#include <lora_driver.h>
#include "uplinkHandler.h"


// define priorities 
#define UPLINK_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )


//define task stack for each task
#define UPLINK_TASK_STACK			(configMINIMAL_STACK_SIZE )


void create_operations(void){


	// LoRaWAN
	uplink_handler_create(UPLINK_TASK_PRIORITY, UPLINK_TASK_STACK);
	
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
	// Create tasks
	create_operations();

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
	printf("Program Started!!\n");
	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.

	while (1)
	{
	}
}

