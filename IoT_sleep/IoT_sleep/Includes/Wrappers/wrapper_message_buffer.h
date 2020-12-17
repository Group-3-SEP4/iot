#ifndef WRAPPER_MESSAGE_BUFFER_H_
#define WRAPPER_MESSAGE_BUFFER_H_

#include <ATMEGA_FreeRTOS.h>
#include <message_buffer.h>

size_t _xMessageBufferSend( MessageBufferHandle_t xMessageBuffer,
const void *pvTxData,
size_t xDataLengthBytes,
TickType_t xTicksToWait );

size_t _xMessageBufferReceive( MessageBufferHandle_t xMessageBuffer,
void *pvRxData,
size_t xBufferLengthBytes,
TickType_t xTicksToWait );

#endif /* WRAPPER_MESSAGE_BUFFER_H_ */