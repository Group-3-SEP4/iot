#ifndef DOWNLINKHANDLER_H
#define DOWNLINKHANDLER_H

#include <message_buffer.h>
#include "configuration.h"

void downlink_handler_create(MessageBufferHandle_t messageBuffer, configuration_t configuration);


#endif 