#include <stdint.h>
#include <mh_z19.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>

#include "co2.h"

typedef struct co2_sensor {
	uint16_t value;
	TaskHandle_t task;
} co2_sensor;



