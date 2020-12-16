#include <stddef.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <display_7seg.h>
#include "uplink_handler.h"
#include "definitions.h"
#include "wrapper_messageBuffer.h"
#include "wrapper_task.h"
#include "secure_print.h"

// LoRa keys
#include "secrets.h"

#define CLASS_NAME	"uplink_handler.c"

static bool initialized = false;


static void lora_setup(void)
{
	char _out_buf[100];
	
	lora_driver_returnCode_t rc;
	status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	// Factory reset the transceiver
	s_print("PROD", CLASS_NAME, "FactoryReset >%s<", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));
	
	// Configure to EU868 LoRaWAN standards
	s_print("PROD", CLASS_NAME, "Configure to EU868 >%s<", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);
	s_print("PROD", CLASS_NAME, "Get HWEUI >%s<: %s",lora_driver_mapReturnCodeToText(rc), _out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	s_print("PROD", CLASS_NAME, "Set DevEUI: %s >%s<", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	s_print("PROD", CLASS_NAME, "Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf)));

	// Save all the MAC settings in the transceiver
	s_print("PROD", CLASS_NAME, "Save mac >%s<",lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

	// Enable Adaptive Data Rate
	s_print("PROD", CLASS_NAME, "Set Adaptive Data Rate: ON >%s<", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	s_print("PROD", CLASS_NAME, "Set Receiver Delay: %d ms >%s<", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LORA_OTAA);
		s_print("PROD", CLASS_NAME, "Join Network TriesLeft:%d >%s<", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

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


static void uplink_handler_task( void *pvParameters )
{
	MessageBufferHandle_t buffer_uplink = pvParameters;

	
	if (!initialized){
		// Hardware reset of LoRaWAN transceiver
		lora_driver_resetRn2483(1);
		_vTaskDelay(2);
		lora_driver_resetRn2483(0);
		// Give it a chance to wakeup
		_vTaskDelay(150);

		lora_driver_flushBuffers(); // get rid of first version string from module after reset!

		lora_setup();
		
		initialized = true;
	}
	
	float packages_sent = 0.0;
	
	for(;;)
	{
		
		lora_driver_payload_t payload;
		size_t xReceivedBytes;
		
		xReceivedBytes = _xMessageBufferReceive( buffer_uplink,( void * ) &payload, sizeof(lora_driver_payload_t ), DEF_WAIT_MSG_BUFFER_EMPTY_UPLINK);
		
		if( xReceivedBytes > 0 )
		{
			display_7seg_display(packages_sent++, 0);

			status_leds_shortPuls(led_ST4);  // OPTIONAL
			//lora_driver_sendUploadMessage(false, &uplink_payload);
			lora_driver_returnCode_t return_code = lora_driver_sendUploadMessage(false, &payload);
			char * return_code_str = lora_driver_mapReturnCodeToText(return_code);
			
			s_print("PROD", CLASS_NAME, "Upload Message: >%s<", return_code_str);
			} else {
			s_print("PROD", CLASS_NAME, "Buffer empty looping around.");
		}
	}
}


void uplink_handler_create(MessageBufferHandle_t buffer_uplink)
{
	_xTaskCreate(
	uplink_handler_task
	,  "UplinkHandler"  // A name just for humans
	,  DEF_STACK_UPLINK  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  buffer_uplink
	,  DEF_PRIORITY_TASK_UPLINK  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}

