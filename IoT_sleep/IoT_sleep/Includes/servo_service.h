#ifndef SERVO_SERVICE_H
#define SERVO_SERVICE_H

#include <event_groups.h>
#include "configuration_service.h"
#include "co2_service.h"
#include "ht_service.h"

typedef struct servo_service* servo_service_t;

servo_service_t servo_service_create(uint8_t servo_no, EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready, configuration_service_t configuration_service, co2_service_t co2_service, ht_service_t ht_service);
uint16_t servo_service_get_position(servo_service_t servo_service);
void servo_service_regulate(servo_service_t service);
#endif