#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <limits.h>
#include <FreeRTOSConfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <hal_defs.h>
#include <semphr.h>
#include <string.h>


// define priorities
#define DEF_PRIORITY_TASK_LINK					(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_UPLINK				(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_SENSOR_DATA_HANDLER	(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_CO2					(tskIDLE_PRIORITY + 2)
#define DEF_PRIORITY_TASK_HT					(tskIDLE_PRIORITY + 2)
#define DEF_PRIORITY_TASK_SERVO					(tskIDLE_PRIORITY + 3)


// define task stack for each task
#define DEF_STACK_UPLINK						(configMINIMAL_STACK_SIZE + 200)
#define DEF_STACK_CO2							(configMINIMAL_STACK_SIZE)
#define DEF_STACK_HT							(configMINIMAL_STACK_SIZE)
#define DEF_STACK_DOWNLINK						(configMINIMAL_STACK_SIZE)
#define DEF_STACK_SENSOR_DATA_HANDLER			(configMINIMAL_STACK_SIZE)
#define DEF_STACK_SERVO							(configMINIMAL_STACK_SIZE + 200)


// define measure event groups bit flags
#define DEF_BIT_DATA_COLLECT_CO2				(1 << 0)
#define DEF_BIT_DATA_COLLECT_HT					(1 << 1)
#define DEF_BIT_DATA_COLLECT_SERVO				(1 << 2)
#define DEF_BIT_COLLECT_START					(DEF_BIT_DATA_COLLECT_CO2 | DEF_BIT_DATA_COLLECT_HT | DEF_BIT_DATA_COLLECT_SERVO)


// define data ready event groups bit flags
#define DEF_BIT_DATA_READY_CO2					(1 << 0)
#define DEF_BIT_DATA_READY_HT					(1 << 1)
#define DEF_BIT_DATA_READY_SERVO				(1 << 2)
#define DEF_BIT_DATA_READY						(DEF_BIT_DATA_READY_CO2 | DEF_BIT_DATA_READY_HT | DEF_BIT_DATA_READY_SERVO)

// define time constraints
#define DEF_WAIT_DEFAULT						pdMS_TO_TICKS(200) // default wait time
#define DEF_TASK_DEFAULT_COLLECT_DATA			pdMS_TO_TICKS(10000UL)

#define DEF_FREQUENCY_UPLINK					pdMS_TO_TICKS(300000UL) // Upload message every 5 minutes (300000 ms)

#define DEF_DELAY_TASK_CO2						DEF_TASK_DEFAULT_COLLECT_DATA
#define DEF_DELAY_DRIVER_CO2					pdMS_TO_TICKS(50)
#define DEF_WAIT_MUTEX_CO2						DEF_WAIT_DEFAULT

#define DEF_DELAY_TASK_HT						DEF_TASK_DEFAULT_COLLECT_DATA
#define DEF_DELAY_DRIVER_HT						pdMS_TO_TICKS(50)
#define DEF_WAIT_MUTEX_HT_WRITE					DEF_WAIT_DEFAULT
#define DEF_WAIT_MUTEX_T_READ					DEF_WAIT_DEFAULT
#define DEF_WAIT_MUTEX_H_READ					DEF_WAIT_DEFAULT

#define DEF_DELAY_TASK_SERVO					DEF_TASK_DEFAULT_COLLECT_DATA
#define DEF_WAIT_MUTEX_SERVO_WRITE				DEF_WAIT_DEFAULT
#define DEF_WAIT_MUTEX_SERVO_READ				portMAX_DELAY

// define I/O
#define DEF_PRINT_TO_TERMINAL					true


// define values
#define DEF_DEFAULT_NA_SENSOR					INT_MAX
#define DEF_PROPORTIONAL_GAIN_TEMP				4	


// define EEPROM addresses
#define DEF_MEMLOC_TEMP							(uint16_t*)10
#define DEF_MEMLOC_CO2_MIN						(uint16_t*)12
#define DEF_MEMLOC_CO2_MAX						(uint16_t*)14


// define Message buffer sizes
#define DEF_MESSAGE_BUFFER_UPLINK				sizeof(lora_driver_payload_t)*2
#define DEF_MESSAGE_BUFFER_DOWNLINK				sizeof(lora_driver_payload_t)*2






/*
	SECURE PRINT
	define a variadic macro called s_print, checks if mutex has been setup while check if its allowed to print, take semaphore, printf with concatenate newline, release semaphore.
	Mutex has to be initialized before usage with xSemaphoreCreateMutex() method.
*/
SemaphoreHandle_t mutex_print;
#define s_print(level, tag, msg, ...) do { \
	if (NULL != mutex_print && (DEF_PRINT_TO_TERMINAL == true || strcmp("INFO", level) != 0)){ \
		if (xSemaphoreTake(mutex_print, DEF_WAIT_DEFAULT) == pdTRUE) { \
			printf(level "\t --- [" tag "] :  "  msg "\n", __VA_ARGS__); \
			xSemaphoreGive(mutex_print); \
		} \
	} \
} while (0)






#endif