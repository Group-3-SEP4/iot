#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include <rc_servo.h>
#include "servo.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "co2_sensor.h"
#include "configuration.h"


typedef struct servo {
	uint8_t servoNo;
	uint16_t position;
} servo;


static SemaphoreHandle_t _servo_mutex;

static EventGroupHandle_t _eventGroupMeasure;
static EventGroupHandle_t _eventGroupDataReady;

static EventBits_t _bitMeasureStart;
static EventBits_t _bitDataReady;

static configuration_t _configuration;
static co2_sensor_t _co2_sensor;

uint16_t servo_getPosition(servo_t servo){
	uint16_t _tmpPos = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_servo_mutex, DEF_WAIT_MUTEX_SERVO) == pdTRUE)
	{
		_tmpPos = servo->position;
		_xSemaphoreGive(_servo_mutex);
	}
	return _tmpPos;
}

static double getClaim(uint16_t currentValue, uint16_t setpoint, uint16_t p_gain){
	double claim = 0;
	if (p_gain > 0){
		claim = (((double)(currentValue - setpoint) /(double) p_gain) * 100);
		if (claim < 0){
			claim = 0;
		} else if (claim > 100){
			claim = 100;
		}
	} 
	return claim;
}

static int8_t calculate_co2Claim(){
	uint16_t setpoint = configuration_getMinCo2(_configuration);
	uint16_t p_gain = (configuration_getMaxCo2(_configuration) - configuration_getMinCo2(_configuration));
	uint16_t currentValue = co2_getMeasurement(_co2_sensor);
	int8_t claim = getClaim(currentValue, setpoint, p_gain);
	if (DEF_PRINT_TO_TERMINAL){
		printf("CO2 claim: current co2 value = %i, setpoint = %i, gain = %i, resulting claim: %i pct.\n", currentValue, setpoint, p_gain, claim);
	}
	return claim;
}


static void servo_regulate(servo_t servo){
	if (_xSemaphoreTake (_servo_mutex, DEF_WAIT_MUTEX_SERVO) == pdTRUE) // protect shared data
	{
		int8_t co2Claim = calculate_co2Claim();
		servo->position = co2Claim;
		
		rc_servo_setPosition(servo->servoNo, co2Claim);

		_xSemaphoreGive(_servo_mutex);
		
		if (_xEventGroupGetBits(_eventGroupMeasure) & _bitMeasureStart) // checks eventMeasureStart bits
		{
			_xEventGroupClearBits(_eventGroupMeasure, _bitMeasureStart); // clears eventMeasure bits
			_xEventGroupSetBits(_eventGroupDataReady, _bitDataReady); // sets eventDataReady bits
		}
	}
}


void servo_task(void* pvParameters){
	
	
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_SERVO;
	
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		servo_regulate((servo_t) pvParameters);
	}
}


servo_t servo_create(uint8_t servoNo, EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady, configuration_t config, co2_sensor_t co2){
	
	servo_t _servo = malloc(sizeof(servo_t));
	if (NULL == _servo){
		return NULL;
	}
	
	_configuration = config;
	_co2_sensor = co2;
	
	_servo->servoNo = servoNo;
	_servo->position = 0;
	
	_servo_mutex = xSemaphoreCreateMutex();
	
	_eventGroupMeasure = eventGroupMeasure;
	_eventGroupDataReady = eventGroupDataReady;
	
	_bitMeasureStart = DEF_BIT_MEASURE_START_SERVO;
	_bitDataReady = DEF_BIT_DATA_READY_SERVO;
	
	
	xTaskCreate(
	servo_task,		/* Function that implements the task. */
	"servo task",		/* Text name for the task. */
	DEF_STACK_SERVO,			/* Stack size in words, not bytes. */
	_servo,				/* Parameter passed into the task. */
	DEF_PRIORITY_TASK_SERVO,	/* Priority at which the task is created. */
	NULL					/* Used to pass out the created task's handle. */
	);
	
	return _servo;
}