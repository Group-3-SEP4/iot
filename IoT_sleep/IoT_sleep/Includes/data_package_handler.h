#ifndef DATA_PACKAGE_HANDLER_H
#define DATA_PACKAGE_HANDLER_H

#include <event_groups.h>
#include <message_buffer.h>
#include "co2_service.h"
#include "ht_service.h"
#include "servo_service.h"

void data_package_handler_create(EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready, MessageBufferHandle_t message_buffer_uplink, co2_service_t co2_service, ht_service_t ht_service, servo_service_t servo_service);

#endif