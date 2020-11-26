#ifndef DATAPACKAGEHANDLER_H
#define DATAPACKAGEHANDLER_H

#include <stdint.h>
#include <event_groups.h>
#include "co2_sensor.h"
#include <lora_driver.h>


typedef struct dataPackage* dataPackageHandler_t;

dataPackageHandler_t dataPackageHandler_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, co2_sensor_t co2_sensor);
lora_driver_payload_t dataPackageHandler_getPayload(dataPackageHandler_t dataPackageHandler);

#endif