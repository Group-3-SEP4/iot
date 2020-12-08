/*
* payloadBuilder.h
*
* Created: 02/12/2020 12.35.25
*  Author: agk13
*/

#pragma once

#include <stdint.h>
#include <lora_driver.h>

typedef struct payload_builder *payload_builder_t;

payload_builder_t payload_builder_create(void);

void payload_builder_setCo2_ppm(payload_builder_t self, uint16_t co2_ppm);

void payload_builder_setHumidity(payload_builder_t self, uint16_t humidity);

void payload_builder_setTemperature(payload_builder_t self, uint16_t temperature);

void payload_builder_setServoPosition(payload_builder_t self, uint16_t servo_position);

void payload_builder_getLoRaPayload(payload_builder_t self, lora_driver_payload_t *payload);

