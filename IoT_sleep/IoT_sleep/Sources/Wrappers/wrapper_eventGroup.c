#include <ATMEGA_FreeRTOS.h>
#include "wrapper_eventGroup.h"

inline EventBits_t _xEventGroupGetBits(EventGroupHandle_t xEventGroup)
{
	return xEventGroupGetBits(xEventGroup);
}


inline EventBits_t _xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t bits)
{
	return xEventGroupClearBits(xEventGroup, bits);
}


inline EventBits_t _xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t bits)
{
	return xEventGroupSetBits(xEventGroup, bits);
}

inline EventBits_t _xEventGroupWaitBits(
const EventGroupHandle_t xEventGroup
, const EventBits_t uxBitsToWaitFor
, const BaseType_t xClearOnExit
, const BaseType_t xWaitForAllBits
, TickType_t xTicksToWait){
	return xEventGroupWaitBits(
	xEventGroup
	, uxBitsToWaitFor
	, xClearOnExit
	, xWaitForAllBits
	, xTicksToWait);
}