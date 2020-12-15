#ifndef WRAPPER_SEMAPHORE_H
#define WRAPPER_SEMAPHORE_H

#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t _xSemaphoreCreateMutex(void);
BaseType_t _xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t _xSemaphoreGive(SemaphoreHandle_t xSemaphore);

#endif