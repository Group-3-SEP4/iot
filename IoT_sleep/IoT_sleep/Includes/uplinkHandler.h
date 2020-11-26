#ifndef UPLINKHANDLER_H
#define UPLINKHANDLER_H

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"
#include "dataPackageHandler.h"

//dataPackageHandler is temporary
void uplink_handler_create(dataPackageHandler_t dataPackageHandler);


#endif