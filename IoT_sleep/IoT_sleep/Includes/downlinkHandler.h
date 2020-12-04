#ifndef DOWNLINKHANDLER_H
#define DOWNLINKHANDLER_H
#include "configuration.h"

#include <message_buffer.h>

void downlinkHandler_create(configuration_t configuration, MessageBufferHandle_t messageBuffer);


#endif /* DOWNLINKHANDLER_H */