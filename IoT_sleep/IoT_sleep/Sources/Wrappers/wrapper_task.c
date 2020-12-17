#include <ATMEGA_FreeRTOS.h>
#include "wrapper_task.h"

inline BaseType_t _xTaskCreate(TaskFunction_t pxTaskCode,
const char * const pcName,  /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
const configSTACK_DEPTH_TYPE usStackDepth,
void * const pvParameters,
UBaseType_t uxPriority,
TaskHandle_t * const pxCreatedTask)
{
	return xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

inline TickType_t _xTaskGetTickCount(void){
	return xTaskGetTickCount();
}

inline void _vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement ) {
	vTaskDelayUntil(pxPreviousWakeTime, xTimeIncrement);
}

inline void _vTaskDelay( const TickType_t xTicksToDelay ) {
	vTaskDelay(xTicksToDelay);
}
