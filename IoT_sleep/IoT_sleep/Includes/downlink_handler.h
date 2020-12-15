#ifndef DOWNLINK_HANDLER_H
#define DOWNLINK_HANDLER_H

#include <message_buffer.h>
#include "configuration_service.h"

void downlink_handler_create(MessageBufferHandle_t buffer_downlink, configuration_service_t configuration_service);

#endif 
