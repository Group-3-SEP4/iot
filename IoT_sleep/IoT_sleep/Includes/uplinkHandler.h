#ifndef UPLINKHANDLER_H
#define UPLINKHANDLER_H

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"

void uplink_handler_create(MessageBufferHandle_t messageBuffer);

#endif