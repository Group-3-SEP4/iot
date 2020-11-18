#pragma once

// define priorities
#define DEF_UPLINK_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define DEF_CO2_TASK_PRIORITY			(tskIDLE_PRIORITY + 2)


// define task stack for each task
#define DEF_UPLINK_STACK				(configMINIMAL_STACK_SIZE)
#define DEF_CO2_STACK					(configMINIMAL_STACK_SIZE)


// define measure event groups bit flags
#define DEF_BIT_CO2_MEASSURE			(1 << 0);
#define DEF_MEASSURES_DONE				DEF_BIT_CO2_MEASSURE;


// define data ready event groups bit flags
#define DEF_BIT_CO2_DATA_READY			(1 << 1);
#define DEF_DATA_READY					DEF_BIT_CO2_DATA_READY;

// define time constraints
#define DEF_UPLINK_FREQUENCY			pdMS_TO_TICKS(300000UL); // Upload message every 5 minutes (300000 ms)