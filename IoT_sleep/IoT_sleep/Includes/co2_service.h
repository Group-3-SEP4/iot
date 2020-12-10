/*
 *  Author: Jacob 273962
 */ 
#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H

#include <stdint.h>
#include <event_groups.h>

typedef struct co2_measurement* co2_t;

co2_t co2_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready);
uint16_t co2_service_get_measurement(co2_t sensor);
void co2_service_measure(co2_t sensor);

#endif