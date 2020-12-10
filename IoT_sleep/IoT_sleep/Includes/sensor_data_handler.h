/*
*  Author: Aron
*/

#pragma once
#include "co2_service.h"
#include "payload_builder.h"

void sensor_data_handler_create(MessageBufferHandle_t messageBuffer, co2_t co2_service);

void sensor_data_handler_task_body(payload_builder_t payload_builder);