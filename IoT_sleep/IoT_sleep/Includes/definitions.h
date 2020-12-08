#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <hal_defs.h>
#include <limits.h>
#include <FreeRTOSConfig.h>
#include <stdbool.h>
#include <stdint.h>


// define priorities
#define DEF_PRIORITY_TASK_UPLINK				(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_DOWNLINK				(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_CO2					(tskIDLE_PRIORITY + 2)
#define DEF_PRIORITY_TASK_DATAPACKAGE			(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_SERVO					(tskIDLE_PRIORITY + 3)


// define task stack for each task
#define DEF_STACK_UPLINK						(configMINIMAL_STACK_SIZE + 200)
#define DEF_STACK_DOWNLINK						(configMINIMAL_STACK_SIZE + 200)
#define DEF_STACK_CO2							(configMINIMAL_STACK_SIZE + 200)
#define DEF_STACK_DATAPACKAGE					(configMINIMAL_STACK_SIZE + 200)
#define DEF_STACK_SERVO							(configMINIMAL_STACK_SIZE + 200)


// define measure event groups bit flags
#define DEF_BIT_MEASURE_START_CO2				(1 << 0)
#define DEF_BIT_MEASURE_START_SERVO				(1 << 3)
#define DEF_BIT_MEASURE_START					(DEF_BIT_MEASURE_START_CO2 | DEF_BIT_MEASURE_START_SERVO)


// define data ready event groups bit flags
#define DEF_BIT_DATA_READY_CO2					(1 << 0)
#define DEF_BIT_DATA_READY_SERVO				(1 << 3)
#define DEF_BIT_DATA_READY						(DEF_BIT_DATA_READY_CO2 | DEF_BIT_DATA_READY_SERVO)


// define time constraints
#define DEF_WAIT_DEFAULT						pdMS_TO_TICKS(200) // default wait time

#define DEF_WAIT_MSG_BUFFER_EMPTY_UPLINK		pdMS_TO_TICKS(portMAX_DELAY)

#define DEF_WAIT_MSG_BUFFER_DOWNLINK			pdMS_TO_TICKS(portMAX_DELAY)

#define DEF_DELAY_TASK_CO2						pdMS_TO_TICKS(5000UL)
#define DEF_DELAY_DRIVER_CO2					pdMS_TO_TICKS(300)
#define DEF_WAIT_MUTEX_CO2						DEF_WAIT_DEFAULT

#define DEF_DELAY_TASK_SEND_DATAPACKAGE			pdMS_TO_TICKS(300000UL) // Upload message every 5 minutes (300000UL = 300000 ms)
#define DEF_WAIT_MUTEX_DATAPACKAGE				DEF_WAIT_DEFAULT
#define DEF_WAIT_EVENT_DATA_READY				pdMS_TO_TICKS(portMAX_DELAY)
#define DEF_WAIT_MSG_BUFFER_FULL_DATAPACKGE		DEF_WAIT_DEFAULT


#define DEF_DELAY_TASK_SERVO					DEF_DELAY_TASK_CO2
#define DEF_WAIT_MUTEX_SERVO					DEF_WAIT_DEFAULT


// define print to terminal
#define DEF_PRINT_TO_TERMINAL					true


// define default values
#define DEF_DEFAULT_NA_SENSOR					INT_MAX					


// define EEPROM addresses
#define DEF_MEMLOC_TEMP							(uint16_t*)10
#define DEF_MEMLOC_CO2_MIN						(uint16_t*)12
#define DEF_MEMLOC_CO2_MAX						(uint16_t*)14

#endif