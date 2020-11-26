#ifndef WRAPPER_EVENTGROUP_H
#define WRAPPER_EVENTGROUP_H

#include <event_groups.h>

inline BaseType_t _xEventGroupGetBits(EventGroupHandle_t xEventGroup)
{
	return xEventGroupGetBits(xEventGroup);
}


inline BaseType_t _xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t bits)
{
	return xEventGroupClearBits(xEventGroup, bits);
}


inline BaseType_t _xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t bits)
{
	return xEventGroupSetBits(xEventGroup, bits);
}

inline BaseType_t _xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait ){
	return xEventGroupWaitBits(xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait);
}

#endif
