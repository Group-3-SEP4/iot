#ifndef HUMIDITYTEMPERATURE_SENSOR_H
#define HUMIDITYTEMPERATURE_SENSOR_H

#include <stdint.h>
#include <event_groups.h>

typedef struct ht_sensor* ht_sensor_t;

ht_sensor_t ht_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady);
int16_t ht_getTemperature(ht_sensor_t sensor);
uint16_t ht_getHumidity(ht_sensor_t sensor);
void ht_measure(ht_sensor_t sensor);

#endif
