#ifndef HT_SERVICE_H
#define HT_SERVICE_H

#include <stdint.h>
#include <event_groups.h>

typedef struct ht_service* ht_service_t;

ht_service_t ht_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready);
int16_t ht_service_get_temperature(ht_service_t service);
uint16_t ht_service_get_humidity(ht_service_t service);
void ht_service_measure(ht_service_t service);

#endif
