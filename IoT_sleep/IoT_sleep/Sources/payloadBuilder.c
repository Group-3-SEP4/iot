/*
* payloadBuilder.c
*
* Created: 04/12/2020 20.24.15
*  Author: agk13
*/

#include <stdlib.h>
#include "payloadBuilder.h"
#include "definitions.h"

typedef struct payload_builder{
	uint16_t co2_ppm;
	int16_t temperature;
	uint16_t humidity;
	uint16_t servo_position;
} payload_builder;


payload_builder_t payload_builder_create(void){
	payload_builder_t _new = malloc(sizeof(*_new));

	if (NULL == _new) return NULL;

	_new->co2_ppm = DEF_DEFAULT_NA_SENSOR;
	_new->temperature = DEF_DEFAULT_NA_SENSOR;
	_new->humidity = DEF_DEFAULT_NA_SENSOR;
	_new->servo_position = DEF_DEFAULT_NA_SENSOR;

	return _new;
}

void payload_builder_setCo2_ppm(payload_builder_t self, uint16_t co2_ppm){
	if (NULL == self) return;
	self->co2_ppm = co2_ppm;
}

void payload_builder_setHumidity(payload_builder_t self, uint16_t humidity){
	if (NULL == self) return;
	self->humidity = humidity;
}

void payload_builder_setTemperature(payload_builder_t self, uint16_t temperature){
	if (NULL == self) return;
	self->temperature = temperature;
}

void payload_builder_setServoPosition(payload_builder_t self, uint16_t servo_position){
	if (NULL == self) return;
	self->servo_position = servo_position;
}

void payload_builder_getLoRaPayload(payload_builder_t self, lora_driver_payload_t *payload){
	if (NULL == self) return;
	
	payload->len = 8;
	payload->port_no = 2;
	
	payload->bytes[0] = self->humidity >> 8;
	payload->bytes[1] = self->humidity & 0xFF;
	payload->bytes[2] = self->temperature >> 8;
	payload->bytes[3] = self->temperature & 0xFF;
	payload->bytes[4] = self->co2_ppm >> 8;
	payload->bytes[5] = self->co2_ppm & 0xFF;
	payload->bytes[6] = self->servo_position >> 8;
	payload->bytes[7] = self->servo_position & 0xFF;
}
