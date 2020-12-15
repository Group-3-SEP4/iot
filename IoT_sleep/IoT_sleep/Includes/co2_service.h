#ifndef CO2_SERVICE_H
#define CO2_SERVICE_H
#include <event_groups.h>


typedef struct co2_service* co2_service_t;

co2_service_t co2_service_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready);
uint16_t co2_service_get_measurement(co2_service_t service);
void co2_service_measure(co2_service_t service);

#endif