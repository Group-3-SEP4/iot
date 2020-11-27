#include "configuration.h"
#include "storageHandler.h"
#include "definitions.h"
#include <stdint.h>
#include <stdlib.h>

#define MIN_CO2 0
#define MAX_CO2 1



typedef struct configuration {
	uint16_t temp;
	uint16_t co2Range[];

} configuration_st;


static configuration_t self;

void configuration_create(void) {
	
	self = malloc(sizeof(configuration_st));
	
	self->temp = storageHandler_readTemperature();
	self->co2Range[MIN_CO2] = storageHandler_readCo2Min();
	self->co2Range[MAX_CO2] = storageHandler_readCo2Max();
	
}

uint16_t configuration_getDefaultTemperatur() {
	return self->temp;
}

void configuration_setDefaultTemperatur(uint16_t temp) {
	storangeHandler_writeTemperature(temp);
	self->temp = temp;
}

uint16_t configuration_getMinCo2() {
	return self->co2Range[MIN_CO2];
}

void configuration_setMinCo2(uint16_t min) {
	storangeHandler_writeCo2Min(min);
	self->co2Range[MIN_CO2] = min;
}


uint16_t configuration_getMaxCo2() {
	return self->co2Range[MAX_CO2];
}

void configuration_setMaxCo2(uint16_t max) {
	storangeHandler_writeCo2Max(max);
	self->co2Range[MAX_CO2] = max;
}








