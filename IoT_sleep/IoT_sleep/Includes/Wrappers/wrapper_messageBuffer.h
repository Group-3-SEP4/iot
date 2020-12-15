#ifndef WRAPPER_MESSAGEBUFFER_H
#define WRAPPER_MESSAGEBUFFER_H

#include <stdint.h>
#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>

size_t _xMessageBufferSend(MessageBufferHandle_t messageBufferHandle, void * pvTxData, uint8_t xDataLengthBytes, TickType_t xTicksToWait);
size_t _xMessageBufferReceive(MessageBufferHandle_t messageBufferHandle, void * pvRxData, uint8_t xDataLengthBytes, TickType_t xTicksToWait);

#endif
