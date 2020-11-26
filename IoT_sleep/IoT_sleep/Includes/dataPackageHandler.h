#ifndef DATAPACKAGEHANDLER_H
#define DATAPACKAGEHANDLER_H

#include "co2_sensor.h"

typedef struct dataPackage* dataPackageHandler_t;

dataPackageHandler_t dataPackageHandler_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, MessageBufferHandle_t messageBufferUplink, co2_sensor_t co2_sensor);

#endif