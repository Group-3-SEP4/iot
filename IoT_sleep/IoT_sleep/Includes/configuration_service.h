#ifndef CONFIGURATION_SERVICE_H
#define CONFIGURATION_SERVICE_H

#include <stdint.h>


typedef struct configuration_service* configuration_service_t;

configuration_service_t configuration_service_create(void);

uint16_t configuration_service_get_temperature(configuration_service_t service);
void configuration_service_set_temperature(configuration_service_t service, uint16_t temp);

uint16_t configuration_service_get_min_co2(configuration_service_t service);
void configuration_service_set_min_co2(configuration_service_t service, uint16_t min);

uint16_t configuration_service_get_max_co2(configuration_service_t service);
void configuration_service_set_max_co2(configuration_service_t service, uint16_t max);

#endif


