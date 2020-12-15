#ifndef PAYLOADBUILDER_H
#define PAYLOADBUILDER_H

#include <lora_driver.h>
#include <stdint.h>

lora_driver_payload_t payloadBuilder_getPayload(int16_t tt, uint16_t rh, uint16_t co, uint16_t sPos);

#endif