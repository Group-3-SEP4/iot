/*
*  Author: Aron 264237
*/

#ifndef UPLINKHANDLER_H
#define UPLINKHANDLER_H

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"

void uplink_handler_create(MessageBufferHandle_t message_buffer);

#endif