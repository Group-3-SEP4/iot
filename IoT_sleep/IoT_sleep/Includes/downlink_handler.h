/*
*	Author: Jesper (273961)
*/
#ifndef DOWNLINKHANDLER_H
#define DOWNLINKHANDLER_H
#include "configuration_service.h"

#include <message_buffer.h>

void downlink_handler_create(configuration_t configuration, MessageBufferHandle_t message_buffer);


#endif /* DOWNLINKHANDLER_H */