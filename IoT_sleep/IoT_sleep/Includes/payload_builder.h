#ifndef PAYLOAD_BUILDER_H
#define PAYLOAD_BUILDER_H

#include <lora_driver.h>
#include <stdint.h>

lora_driver_payload_t payload_builder_get_payload(int16_t tt, uint16_t rh, uint16_t co, uint16_t sPos);

#endif
