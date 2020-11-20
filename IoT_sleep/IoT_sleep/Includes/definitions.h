#pragma once

#include <limits.h>

// define priorities
#define DEF_PRIORITY_TASK_UPLINK			(tskIDLE_PRIORITY + 1)
#define DEF_PRIORITY_TASK_CO2				(tskIDLE_PRIORITY + 2)


// define task stack for each task
#define DEF_STACK_UPLINK					(configMINIMAL_STACK_SIZE)
#define DEF_STACK_CO2						(configMINIMAL_STACK_SIZE + 200)


// define measure event groups bit flags
#define DEF_BIT_MEASURE_START_CO2			(1 << 0)
#define DEF_BIT_MEASURE_DONE				DEF_BIT_MEASURE_START_CO2


// define data ready event groups bit flags
#define DEF_BIT_DATA_READY_CO2				(1 << 1)
#define DEF_DATA_READY						DEF_BIT_DATA_READY_CO2


// define time constraints
#define DEF_FREQUENCY_UPLINK				pdMS_TO_TICKS(300000UL) // Upload message every 5 minutes (300000 ms)
#define DEF_DELAY_TASK_CO2					pdMS_TO_TICKS(5000UL)
#define DEF_DELAY_DRIVER_CO2				pdMS_TO_TICKS(300)
#define DEF_WAIT_MUTEX_CO2					pdMS_TO_TICKS(200)


// define hardware I/O
#define DEF_IO_PORT_CO2						ser_USART3


// define default values
#define DEF_DEFAULT_NA_SENSOR				INT_MAX

