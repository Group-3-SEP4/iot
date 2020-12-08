#ifndef SERVO_H
#define SERVO_H

#include "configuration.h"
#include "co2_sensor.h"

typedef struct servo* servo_t;

servo_t servo_create(uint8_t servoNo, EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, configuration_t config, co2_sensor_t co2);
uint16_t servo_getPosition(servo_t servo);
#endif