/*
*  Author: Aron
*/

#pragma once
#include "co2_service.h"
#include "payload_builder.h"
#include "ht_service.h"
#include "servo_service.h"

void sensor_data_handler_create(
MessageBufferHandle_t messageBuffer,
EventGroupHandle_t event_group_data_collect,
EventGroupHandle_t event_group_data_ready,
co2_t co2_service,
ht_t ht_service,
servo_t servo_service);

void sensor_data_handler_task_body(payload_builder_t payload_builder);
