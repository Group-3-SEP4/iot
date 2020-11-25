#ifndef UPLINKHANDLER_H
#define UPLINKHANDLER_H

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"

// co2 sensor is temporary
void uplink_handler_create(co2_sensor_t co2Sensor);


#endif