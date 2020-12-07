
#include <ATMEGA_FreeRTOS.h>
#include "wrapper_semaphore.h"

inline QueueHandle_t _xSemaphoreCreateMutex(){
	return xSemaphoreCreateMutex();
}


inline BaseType_t _xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait)
{
	return xSemaphoreTake(xSemaphore, xTicksToWait);
}


inline BaseType_t _xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
	return xSemaphoreGive(xSemaphore);
}
