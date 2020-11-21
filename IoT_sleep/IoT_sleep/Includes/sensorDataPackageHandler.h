/*
* sensorDataPackageHandler.h
*
* Created: 21/11/2020 10.11.44
*  Author: agk13
*/

#include "stdint.h"
#include <lora_driver.h>

#pragma once

typedef struct SensorDataPackageHandler *sensor_data_package_handler_t;

sensor_data_package_handler_t sensorDataPackageHandler_create(void);

void sensorDataPackageHandler_destroy(sensor_data_package_handler_t self);

void sensorDataPackageHandler_setCo2_ppm(sensor_data_package_handler_t self, uint16_t ppm);

void sensorDataPackageHandler_setHumidity(sensor_data_package_handler_t self, uint16_t hum);

void sensorDataPackageHandler_setTemperature(sensor_data_package_handler_t self, uint16_t temp);

void sensorDataPackageHandler_setServoPosition(sensor_data_package_handler_t self, uint16_t s_pos);

lora_driver_payload_t* sensorDataPackageHandler_getLoRaPayload(sensor_data_package_handler_t self);