/*
 *  Author: Jacob 273962
 */ 
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
#include "secure_print.h"
#include "servo_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "co2_service.h"
#include "ht_service.h"
#include "configuration_service.h"

#define CLASS_NAME "servo_service.c"

typedef struct servo {
	uint8_t servo_no;
	uint16_t position;
} servo;

static SemaphoreHandle_t _mutex;

static EventGroupHandle_t _event_group_data_collect;
static EventGroupHandle_t _event_group_data_ready;

static configuration_t _configuration_service;
static co2_t _co2_service;
static ht_t _ht_service;

uint16_t servo_service_get_position(servo_t servo){
	uint16_t tmp_pos = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_SERVO_READ) == pdTRUE)
	{
		tmp_pos = servo->position;
		_xSemaphoreGive(_mutex);
	}
	return tmp_pos;
}

static double calculate_claim(uint16_t current_value, uint16_t setpoint, uint16_t p_gain){
	double claim = 0.0;
	if (p_gain > 0){
		claim = (((double)(current_value - setpoint) /(double) p_gain) * 100.0);
		if (claim < 0.0){
			claim = 0.0;
		}
		if (claim > 100.0){
			claim = 100.0;
		}
	}
	return claim;
}

static int8_t get_co2_claim(){
	uint16_t setpoint = configuration_service_get_min_co2(_configuration_service);
	uint16_t p_gain = (configuration_service_get_max_co2(_configuration_service) - configuration_service_get_min_co2(_configuration_service));
	uint16_t currentValue = co2_service_get_measurement(_co2_service);
	int8_t claim = calculate_claim(currentValue, setpoint, p_gain);
	return claim;
}

static int8_t get_temp_claim(){
	uint16_t setpoint = configuration_service_get_temperature(_configuration_service);
	uint16_t p_gain = DEF_PROPORTIONAL_GAIN_TEMP;
	uint16_t currentValue = ht_service_get_temperature(_ht_service);
	int8_t claim = calculate_claim(currentValue, setpoint, p_gain);
	return claim;
}


static void servo_regulate(servo_t servo){
	int8_t co2_claim = get_co2_claim();
	int8_t temp_claim = get_temp_claim();
	int8_t max_claim = 0;
	
	if (co2_claim > temp_claim){
		max_claim = co2_claim;
		} else {
		max_claim = temp_claim;
	}
	
	rc_servo_setPosition(servo->servo_no, max_claim);
	
	if (_xSemaphoreTake (_mutex, DEF_WAIT_MUTEX_SERVO_WRITE) == pdTRUE) // protect shared data
	{
		servo->position = max_claim;
		
		_xSemaphoreGive(_mutex);
		
		if (_xEventGroupGetBits(_event_group_data_collect) & DEF_BIT_DATA_COLLECT_SERVO) // checks eventMeasureStart bits
		{
			_xEventGroupClearBits(_event_group_data_collect, DEF_BIT_DATA_COLLECT_SERVO); // clears eventMeasure bits
			_xEventGroupSetBits(_event_group_data_ready, DEF_BIT_DATA_READY_SERVO); // sets eventDataReady bits
		}
	}
	
	s_print("INFO", CLASS_NAME, "Regulator claim: TT %i, CO2 %i, Max %i pct.",temp_claim, co2_claim, max_claim);
	
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


servo_t servo_service_create(uint8_t servo_no, EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready, configuration_t configuration_service, co2_t co2_service, ht_t ht_service){
	
	servo_t _servo = malloc(sizeof(servo_t));
	if (NULL == _servo){
		return NULL;
	}
	
	_configuration_service = configuration_service;
	_co2_service = co2_service;
	_ht_service = ht_service;
	
	_servo->servo_no = servo_no;
	_servo->position = 0;
	
	_mutex = xSemaphoreCreateMutex();
	
	_event_group_data_collect = event_group_data_collect;
	_event_group_data_ready = event_group_data_ready;
	
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