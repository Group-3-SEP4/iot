#include "configuration.h"

#define MAX_CO2 1
#define MIN_CO2 0


typedef struct configuration {
	uint16_t temp;
	uint16_t[] co2Range;

} configuration


configuration_t configuration_create(void) {
	
}

uint16_t configuration_getDefaultTemperatur() {
	return configuration->temp;
}

void configuration_setDefaultTemperatur(uint16_t temp) {
	configuration->temp = temp;
}

uint16_t configuration_getMaxCo2() {
	return configuration->co2Range[MAX_CO2];
}

void configuration_setMaxCo2(uint16_t max) {
	configuration->co2Range[MAX_CO2] = max;
}

uint16_t configuration_getMinCo2() {
	return configuration->co2Range[MIN_CO2];
}

void configuration_setMinCo2(uint16_t min) {
	configuration->co2Range[MIN_CO2] = min;
}







