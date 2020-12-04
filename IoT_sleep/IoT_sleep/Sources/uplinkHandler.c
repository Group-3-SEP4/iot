#include <stddef.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <display_7seg.h>
#include "definitions.h"

// LoRa keys 
#include "secrets.h"

static bool initialized = false;
static char _out_buf[100];

void uplink_handler_task( void *pvParameters );

// co2 sensor should be removed when message buffer is implemented 
#include "co2_sensor.h"
static co2_sensor_t _co2Sensor;
void uplink_handler_create(co2_sensor_t co2Sensor)
{
	_co2Sensor = co2Sensor;
	
	xTaskCreate(
	uplink_handler_task
	,  (const portCHAR *)"UplinkHandler"  // A name just for humans
	,  DEF_STACK_UPLINK  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  DEF_PRIORITY_TASK_UPLINK  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}


static lora_driver_payload_t _uplink_payload; // should be passed as argument from buffer

/*-----------------------------------------------------------*/
void uplink_handler_task( void *pvParameters )
{
	if (!initialized){
		// Hardware reset of LoRaWAN transceiver
		lora_driver_resetRn2483(1);
		vTaskDelay(2);
		lora_driver_resetRn2483(0);
		// Give it a chance to wakeup
		vTaskDelay(150);

		lora_driver_flushBuffers(); // get rid of first version string from module after reset!
		
		initialized = true;
	}
	
	_uplink_payload.len = 8;		// should be passed as argument from buffer
	_uplink_payload.port_no = 2;	// should be passed as argument from buffer

	 TickType_t xLastWakeTime;  // maybe this should be moved to other place where time can be handled 
	 const TickType_t xFrequency = DEF_FREQUENCY_UPLINK; 
	 xLastWakeTime = xTaskGetTickCount();
	 
	 float packagesSent = 0.0;
	 
	for(;;) // task might need to loop else where, so it can be called by buffer
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		// Some dummy payload // should be passed as argument from buffer
		uint16_t hum = DEF_DEFAULT_NA_SENSOR; // Dummy humidity
		int16_t temp = DEF_DEFAULT_NA_SENSOR; // Dummy temp
		uint16_t co2_ppm = co2_getMeasurement(_co2Sensor); 
		uint16_t serv = DEF_DEFAULT_NA_SENSOR;

		_uplink_payload.bytes[0] = hum >> 8; 
		_uplink_payload.bytes[1] = hum & 0xFF;
		_uplink_payload.bytes[2] = temp >> 8;
		_uplink_payload.bytes[3] = temp & 0xFF;
		_uplink_payload.bytes[4] = co2_ppm >> 8;
		_uplink_payload.bytes[5] = co2_ppm & 0xFF;
		_uplink_payload.bytes[6] = serv >> 8;
		_uplink_payload.bytes[7] = serv & 0xFF;
		
		
		display_7seg_display(++packagesSent, 0);

		status_leds_shortPuls(led_ST4);  // OPTIONAL
		printf("Upload Message: CO2 value: %i ppm - >%s<\n", co2_ppm, lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload)));
	}
}
