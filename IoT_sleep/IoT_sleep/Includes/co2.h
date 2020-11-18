#pragma once

typedef struct co2_sensor* co2_sensor_t;

co2_sensor_t co2_create();
void co2_destroy(co2_sensor_t* sensor);
void co2_getValue(co2_sensor_t sensor);

