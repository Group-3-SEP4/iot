#ifndef DATAPACKAGEHANDLER_H
#define DATAPACKAGEHANDLER_H

#include "co2_sensor.h"
#include "ht_sensor.h"
#include "servo.h"

typedef struct dataPackage* dataPackageHandler_t;

dataPackageHandler_t dataPackageHandler_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, MessageBufferHandle_t messageBufferUplink, co2_sensor_t co2_sensor, ht_sensor_t ht_sensor, servo_t servo);

#endif