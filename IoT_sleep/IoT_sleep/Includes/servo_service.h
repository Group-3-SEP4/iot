/*
 *  Author: Jacob 273962
 */ 
#ifndef SERVO_SERVICE_H
#define SERVO_SERVICE_H

#include "configuration_service.h"
#include "co2_service.h"
#include "ht_service.h"

typedef struct servo* servo_t;

servo_t servo_service_create(
uint8_t servo_no, 
EventGroupHandle_t event_group_data_collect, 
EventGroupHandle_t event_group_data_ready, 
configuration_t configuration_service, 
co2_t co2_service, 
ht_t ht_service);

uint16_t servo_service_get_position(servo_t servo);
#endif