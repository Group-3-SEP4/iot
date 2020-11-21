#include <stddef.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <display_7seg.h>
#include "definitions.h"
#include "sensorDataPackageHandler.h"

// LoRa keys
#include "secrets.h"

static bool initialized = false;
static char _out_buf[100];
static sensor_data_package_handler_t packageHandler;

void uplink_handler_task( void *pvParameters );

// co2 sensor should be removed when message buffer is implemented
#include "co2.h"
static co2_sensor_t _co2Sensor;
void uplink_handler_create(co2_sensor_t co2Sensor)
{
	_co2Sensor = co2Sensor;
	
	xTaskCreate(
	uplink_handler_task
	,  (const portCHAR *)"UplinkHandler"  // A name just for humans
	,  DEF_STACK_UPLINK + 200  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  DEF_PRIORITY_TASK_UPLINK  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}

static void _lora_setup(void)
{
	lora_driver_returnCode_t rc;
	status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	// Factory reset the transceiver
	printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));
	
	// Configure to EU868 LoRaWAN standards
	printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);
	printf("Get HWEUI >%s<: %s\n",lora_driver_mapReturnCodeToText(rc), _out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf)));

	// Save all the MAC settings in the transceiver
	printf("Save mac >%s<\n",lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

	// Enable Adaptive Data Rate
	printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LORA_OTAA);
		printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

		if ( rc != LORA_ACCEPTED)
		{
			display_7seg_display((float)maxJoinTriesLeft, 0);
			// Make the red led pulse to tell something went wrong
			status_leds_longPuls(led_ST1); // OPTIONAL
			// Wait 5 sec and lets try again
			vTaskDelay(pdMS_TO_TICKS(5000UL));
		}
		else
		{
			break;
		}
	} while (--maxJoinTriesLeft);

	if (rc == LORA_ACCEPTED)
	{
		display_7seg_display(1111.0, 0);
		// Connected to LoRaWAN :-)
		// Make the green led steady
		status_leds_ledOn(led_ST2); // OPTIONAL
	}
	else
	{
		display_7seg_display(9999.0, 0);
		// Something went wrong
		// Turn off the green led
		status_leds_ledOff(led_ST2); // OPTIONAL
		// Make the red led blink fast to tell something went wrong
		status_leds_fastBlink(led_ST1); // OPTIONAL

		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
}

//static lora_driver_payload_t _uplink_payload; // should be passed as argument from buffer

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

		_lora_setup();
		
		
		initialized = true;
	}
	
	packageHandler = sensorDataPackageHandler_create();

	TickType_t xLastWakeTime;  // maybe this should be moved to other place where time can be handled
	const TickType_t xFrequency = DEF_FREQUENCY_UPLINK;
	xLastWakeTime = xTaskGetTickCount();
	
	float packagesSent = 0.0;
	
	for(;;) // task might need to loop else where, so it can be called by buffer
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		uint16_t co2_ppm = co2_getMeasurement(_co2Sensor);
		
		sensorDataPackageHandler_setCo2_ppm(packageHandler, co2_ppm);
		// Data which has not been set defaults to DEF_DEFAULT_NA_SENSOR

		lora_driver_payload_t* _uplink_payload = sensorDataPackageHandler_getLoRaPayload(packageHandler);
		
		
		display_7seg_display(++packagesSent, 0);

		status_leds_shortPuls(led_ST4);  // OPTIONAL
		printf("Upload Message: CO2 value: %i ppm - >%s<\n", co2_ppm, lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, _uplink_payload)));
	}
}
