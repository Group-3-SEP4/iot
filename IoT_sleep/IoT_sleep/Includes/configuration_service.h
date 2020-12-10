/*
*  Author: Jesper 273961
*/

#pragma once

#include <stdint.h>

typedef struct configuration* configuration_t;

configuration_t configuration_service_create(void);

uint16_t configuration_service_get_default_temperatur(configuration_t self);
void configuration_service_set_default_temperatur(configuration_t self, uint16_t temp);

uint16_t configuration_service_get_min_co2(configuration_t self);
void configuration_service_set_min_co2(configuration_t self, uint16_t min);

uint16_t configuration_service_get_max_co2(configuration_t self);
void configuration_service_set_max_co2(configuration_t selv, uint16_t max);

