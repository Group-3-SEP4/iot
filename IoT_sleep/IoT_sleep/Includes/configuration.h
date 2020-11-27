#pragma once

#include <stdint.h>



typedef struct configuration* configuration_t;



void configuration_create(void);

uint16_t configuration_getDefaultTemperatur();
void configuration_setDefaultTemperatur(uint16_t temp);

uint16_t configuration_getMinCo2();
void configuration_setMinCo2(uint16_t min);

uint16_t configuration_getMaxCo2();
void configuration_setMaxCo2(uint16_t max);

