#ifndef HUMIDITYTEMPERATURE_SERVICE_H
#define HUMIDITYTEMPERATURE_SERVICE_H

#include <stdint.h>
#include <event_groups.h>

typedef struct ht_measurement* ht_t;

ht_t ht_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready);
int16_t ht_get_temperature(ht_t sensor);
uint16_t ht_get_humidity(ht_t sensor);
void ht_measure(ht_t sensor);



#endif
