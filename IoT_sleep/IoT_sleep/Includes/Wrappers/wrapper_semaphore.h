#ifndef WRAPPER_SEMAPHORE_H
#define WRAPPER_SEMAPHORE_H

#include <semphr.h>

inline BaseType_t _xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait)
{
	return xSemaphoreTake(xSemaphore, xTicksToWait);
}


inline BaseType_t _xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
	return xSemaphoreGive(xSemaphore);
}

#endif