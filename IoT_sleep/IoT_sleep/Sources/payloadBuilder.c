#include <stdint.h>
#include <lora_driver.h>
#include "payloadBuilder.h"

static lora_driver_payload_t payload;

lora_driver_payload_t payloadBuilder_getPayload(int16_t tt, uint16_t rh, uint16_t co, uint16_t sPos){
	payload.len = 8;
	payload.port_no = 2;
	
	payload.bytes[0] = rh >> 8;
	payload.bytes[1] = rh & 0xFF;
	payload.bytes[2] = tt >> 8;
	payload.bytes[3] = tt & 0xFF;
	payload.bytes[4] = co >> 8;
	payload.bytes[5] = co & 0xFF;
	payload.bytes[6] = sPos >> 8;
	payload.bytes[7] = sPos & 0xFF;
	
	return payload;
}
