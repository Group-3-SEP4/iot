#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H

typedef struct co2_sensor* co2_sensor_t;

co2_sensor_t co2_create(EventGroupHandle_t eventGroupMeasure, EventGroupHandle_t eventGroupDataReady);
uint16_t co2_getMeasurement(co2_sensor_t sensor);
void co2_measure(co2_sensor_t sensor);

#endif