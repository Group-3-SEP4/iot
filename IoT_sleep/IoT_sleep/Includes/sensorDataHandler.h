/*
 * sensorDataHandler.h
 *
 * Created: 04/12/2020 21.08.56
 *  Author: agk13
 */ 

#pragma once
#include "co2_sensor.h"

void sensor_data_handler_create(MessageBufferHandle_t messageBuffer, co2_sensor_t co2Sensor);
