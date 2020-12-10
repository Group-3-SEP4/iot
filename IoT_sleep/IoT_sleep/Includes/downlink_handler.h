/*
*	Author: Jesper (273961)
*/
#ifndef DOWNLINK_HANDLER_H
#define DOWNLINK_HANDLER_H
#include "configuration_service.h"

#include <message_buffer.h>

void downlink_handler_create(configuration_t configuration, MessageBufferHandle_t message_buffer);
void downlink_handler(void* message_buffer);
void downlink_handler_task(void* message_buffer);

#endif /* DOWNLINKHANDLER_H */