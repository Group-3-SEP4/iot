/*
 * uplinkHandler.h
 *
 * Created: 15/11/2020 15.23.07
 *  Author: Jacob
 */ 

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"


void uplink_handler_create(UBaseType_t priority, UBaseType_t stack);
void uplink_handler_task();
