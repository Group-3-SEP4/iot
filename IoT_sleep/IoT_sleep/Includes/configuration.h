#pragma once

#include <stdint.h>



typedef struct configuration* configuration_t;



configuration_t configuration_create(void);

uint16_t configuration_getDefaultTemperatur(configuration_t self);
void configuration_setDefaultTemperatur(configuration_t self, uint16_t temp);

uint16_t configuration_getMinCo2(configuration_t self);
void configuration_setMinCo2(configuration_t self, uint16_t min);

uint16_t configuration_getMaxCo2(configuration_t self);
void configuration_setMaxCo2(configuration_t selv, uint16_t max);

