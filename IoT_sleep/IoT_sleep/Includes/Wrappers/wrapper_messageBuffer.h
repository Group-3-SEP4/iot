#ifndef WRAPPER_MESSAGEBUFFER_H
#define WRAPPER_MESSAGEBUFFER_H

#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>

BaseType_t _xMessageBufferSend(MessageBufferHandle_t messageBufferHandle, void * pvTxData, uint8_t xDataLengthBytes, TickType_t xTicksToWait);
BaseType_t _xMessageBufferReceive(MessageBufferHandle_t messageBufferHandle, void * pvRxData, uint8_t xDataLengthBytes, TickType_t xTicksToWait);

#endif
