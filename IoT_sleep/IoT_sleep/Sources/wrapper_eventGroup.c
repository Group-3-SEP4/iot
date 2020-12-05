#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>

inline BaseType_t _xEventGroupGetBits(EventGroupHandle_t xEventGroup)
{
	return xEventGroupGetBits(xEventGroup);
}


inline BaseType_t _xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t bits)
{
	return xEventGroupClearBits(xEventGroup, bits);
}


inline BaseType_t _xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t bits)
{
	return xEventGroupSetBits(xEventGroup, bits);
}
