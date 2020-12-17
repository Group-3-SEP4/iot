#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <rc_servo.h>
#include "servo_service.h"
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "wrapper_eventGroup.h"
#include "wrapper_task.h"
#include "co2_service.h"
#include "ht_service.h"
#include "configuration_service.h"
#include "secure_print.h"

#define CLASS_NAME	"servo.c"

typedef struct servo_service {
	uint8_t servo_no;
	uint16_t position;
	SemaphoreHandle_t mutex;
	EventGroupHandle_t event_ready;
	EventGroupHandle_t event_collect;
} servo_service_st;

static configuration_service_t configuration;
static co2_service_t co2;
static ht_service_t ht;

uint16_t servo_service_get_position(servo_service_t service){
	uint16_t tmp_pos = DEF_DEFAULT_NA_SENSOR;
	if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_SERVO_READ) == pdTRUE)
	{
		tmp_pos = service->position;
		_xSemaphoreGive(service->mutex);
	}
	return tmp_pos;
}

static double servo_service_calculate_claim(uint16_t current_value, uint16_t setpoint, uint16_t p_gain){
	double claim = 0.0;
	if (p_gain > 0){
		claim = (((double)current_value - (double)setpoint) /(double) p_gain) * 100.0;
		if (claim < 0.0){
			claim = 0.0;
		}
		if (claim > 100.0){
			claim = 100.0;
		}
	} 
	return claim;
}

static int8_t servo_service_get_co2_claim(){
	uint16_t setpoint = configuration_service_get_min_co2(configuration);
	uint16_t p_gain = (configuration_service_get_max_co2(configuration) - configuration_service_get_min_co2(configuration));
	uint16_t current_value = co2_service_get_measurement(co2);
	int8_t claim = servo_service_calculate_claim(current_value, setpoint, p_gain);
	return claim;
}

static int8_t servo_service_get_temp_claim(){
	uint16_t setpoint = configuration_service_get_temperature(configuration);
	uint16_t p_gain = DEF_PROPORTIONAL_GAIN_TEMP;
	uint16_t current_value = ht_service_get_temperature(ht);
	int8_t claim = servo_service_calculate_claim(current_value, setpoint, p_gain);
	return claim;
}

void servo_service_regulate(servo_service_t service){
	if (_xSemaphoreTake (service->mutex, DEF_WAIT_MUTEX_SERVO_WRITE) == pdTRUE) // protect shared data
	{
		int8_t co2_claim = servo_service_get_co2_claim();
		int8_t temp_claim = servo_service_get_temp_claim();
		int8_t max_claim = 0;
		
		if (co2_claim > temp_claim){
			max_claim = co2_claim;
		} else {
			max_claim = temp_claim;
		}
		
		service->position = max_claim;
		rc_servo_setPosition(service->servo_no, max_claim);

		_xSemaphoreGive(service->mutex);
		
		if (_xEventGroupGetBits(service->event_collect) & DEF_BIT_DATA_COLLECT_SERVO) // checks eventMeasureStart bits
		{
			_xEventGroupClearBits(service->event_collect, DEF_BIT_DATA_COLLECT_SERVO); // clears eventMeasure bits
			_xEventGroupSetBits(service->event_ready, DEF_BIT_DATA_READY_SERVO); // sets eventDataReady bits
		}
		s_print("INFO", CLASS_NAME, "Regulator: TT %i, CO2 %i, Max %i pct.",temp_claim, co2_claim, max_claim);
	}
}

static void servo_service_task(void* pvParameters){
	
	
	TickType_t xLastWakeTime = _xTaskGetTickCount();
	const TickType_t xFrequency = DEF_DELAY_TASK_SERVO;
	
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency); // execution delay must be defined as first
		servo_service_regulate((servo_service_t) pvParameters);
	}
}

servo_service_t servo_service_create(uint8_t servo_no, EventGroupHandle_t event_group_data_collect, EventGroupHandle_t event_group_data_ready, configuration_service_t configuration_service, co2_service_t co2_service, ht_service_t ht_service){
	
	servo_service_t service = malloc(sizeof(servo_service_st));
	if (NULL == service){
		return NULL;
	}
	
	configuration = configuration_service;
	co2 = co2_service;
	ht = ht_service;
	
	service->servo_no = servo_no;
	service->position = 0;
	
	service->mutex = _xSemaphoreCreateMutex();
	
	service->event_collect = event_group_data_collect;
	service->event_ready = event_group_data_ready;
	
	_xTaskCreate(
	servo_service_task,		/* Function that implements the task. */
	"servo task",		/* Text name for the task. */
	DEF_STACK_SERVO,			/* Stack size in words, not bytes. */
	service,				/* Parameter passed into the task. */
	DEF_PRIORITY_TASK_SERVO,	/* Priority at which the task is created. */
	NULL					/* Used to pass out the created task's handle. */
	);
	
	return service;
}