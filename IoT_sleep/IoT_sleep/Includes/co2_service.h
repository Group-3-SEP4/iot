/*
 *  Author: Jacob 273962
 */ 
#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H

#include <stdint.h>
#include <event_groups.h>

typedef struct co2_measurement* co2_service_t;

co2_service_t co2_service_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady);
uint16_t co2_service_get_measurement(co2_service_t sensor);
void co2_service_measure(co2_service_t sensor);

#endif