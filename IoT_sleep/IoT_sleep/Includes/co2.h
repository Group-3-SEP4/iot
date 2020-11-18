#pragma once

#include <stdint.h>
#include <event_groups.h>

typedef struct co2_sensor* co2_sensor_t;

co2_sensor_t co2_create(EventGroupHandle_t eventGroupMeassure, EventGroupHandle_t eventGroupDataReady);
void co2_destroy(co2_sensor_t* sensor);
uint16_t co2_getMeassure(co2_sensor_t sensor);
