/*
*  Author: Aron 264237
*/

#pragma once

#include <stdint.h>
#include <lora_driver.h>

typedef struct payload_builder *payload_builder_t;

payload_builder_t payload_builder_create(void);

void payload_builder_set_co2_ppm(payload_builder_t self, uint16_t co2_ppm);

void payload_builder_set_humidity(payload_builder_t self, uint16_t humidity);

void payload_builder_set_temperature(payload_builder_t self, uint16_t temperature);

void payload_builder_set_servo_position(payload_builder_t self, uint16_t servo_position);

void payload_builder_get_lora_payload(payload_builder_t self, lora_driver_payload_t *payload);
