#ifndef WRAPPER_EVENTGROUP_H
#define WRAPPER_EVENTGROUP_H

#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>

EventBits_t _xEventGroupGetBits(EventGroupHandle_t xEventGroup);
EventBits_t _xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t bits);
EventBits_t _xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t bits);
EventBits_t _xEventGroupWaitBits(const EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait);

#endif
