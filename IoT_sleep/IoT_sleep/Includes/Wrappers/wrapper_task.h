#ifndef WRAPPER_TASK_H
#define WRAPPER_TASK_H

#include <task.h>

BaseType_t _xTaskCreate(TaskFunction_t pxTaskCode,
									const char * const pcName,  /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
									const configSTACK_DEPTH_TYPE usStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									TaskHandle_t * const pxCreatedTask);

TickType_t _xTaskGetTickCount(void);
void _vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement );
void _vTaskDelay( const TickType_t xTicksToDelay );

#endif