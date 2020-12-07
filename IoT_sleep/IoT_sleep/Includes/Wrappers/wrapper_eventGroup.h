#ifndef WRAPPER_EVENTGROUP_H
#define WRAPPER_EVENTGROUP_H

#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>

BaseType_t _xEventGroupGetBits(EventGroupHandle_t xEventGroup);
BaseType_t _xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t bits);
BaseType_t _xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t bits);

#endif
