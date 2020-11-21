/*
* sensorDataPackageHandler.c
*
* Created: 21/11/2020 10.12.12
*  Author: agk13
*/

#include "sensorDataPackageHandler.h"
#include <stdint.h>
#include <stdlib.h>
#include "definitions.h"


typedef struct SensorDataPackageHandler {
	uint16_t co2_ppm;
	int16_t temperature;
	uint16_t humidity;
	uint16_t servo_position;
} SensorDataPackageHandler;

sensor_data_package_handler_t sensorDataPackageHandler_create(void){
	sensor_data_package_handler_t _newHandler = calloc(sizeof(struct SensorDataPackageHandler), 1);

	if (NULL == _newHandler) return NULL;

	_newHandler->co2_ppm = DEF_DEFAULT_NA_SENSOR;
	_newHandler->temperature = DEF_DEFAULT_NA_SENSOR;
	_newHandler->humidity = DEF_DEFAULT_NA_SENSOR;
	_newHandler->servo_position = DEF_DEFAULT_NA_SENSOR;

	return _newHandler;
}

void sensorDataPackageHandler_destroy(sensor_data_package_handler_t self){
	if (self == NULL) return;
	free(self);
}

void sensorDataPackageHandler_setCo2_ppm(sensor_data_package_handler_t self, uint16_t ppm){
	if (self == NULL) return;
	self->co2_ppm = ppm;
}

void sensorDataPackageHandler_setHumidity(sensor_data_package_handler_t self, uint16_t hum){
	if (self == NULL) return;
	self->humidity = hum;
}

void sensorDataPackageHandler_setTemperature(sensor_data_package_handler_t self, uint16_t temp){
	if (self == NULL) return;
	self->temperature = temp;
}

void sensorDataPackageHandler_setServoPosition(sensor_data_package_handler_t self, uint16_t s_pos){
	if (self == NULL) return;
	self->servo_position = s_pos;
}

lora_driver_payload_t* sensorDataPackageHandler_getLoRaPayload(sensor_data_package_handler_t self){
	if (self == NULL) return NULL;
	
	lora_driver_payload_t * _uplink_payload = calloc(sizeof(struct lora_driver_payload), 1);
	
	_uplink_payload->len = 8;
	_uplink_payload->port_no = 2;
	
	_uplink_payload->bytes[0] = self->humidity >> 8;
	_uplink_payload->bytes[1] = self->humidity & 0xFF;
	_uplink_payload->bytes[2] = self->temperature >> 8;
	_uplink_payload->bytes[3] = self->temperature & 0xFF;
	_uplink_payload->bytes[4] = self->co2_ppm >> 8;
	_uplink_payload->bytes[5] = self->co2_ppm & 0xFF;
	_uplink_payload->bytes[6] = self->servo_position >> 8;
	_uplink_payload->bytes[7] = self->servo_position & 0xFF;
	
	return _uplink_payload;
}
