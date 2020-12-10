/*
*  Author: Aron 264237
*/

#ifndef UPLINKHANDLER_H
#define UPLINKHANDLER_H

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"

void uplink_handler_create(MessageBufferHandle_t message_buffer);

void uplink_handler_task_body(MessageBufferHandle_t uplink_message_buffer, float packagesSent);

#endif