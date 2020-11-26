#ifndef WRAPPER_MESSAGEBUFFER_H
#define WRAPPER_MESSAGEBUFFER_H

#include <message_buffer.h>

inline BaseType_t _xMessageBufferSend(MessageBufferHandle_t messageBufferHandle, void * pvTxData, uint8_t xDataLengthBytes, TickType_t xTicksToWait){
	return xMessageBufferSend(messageBufferHandle, pvTxData, xDataLengthBytes, xTicksToWait);
}

inline BaseType_t _xMessageBufferReceive(MessageBufferHandle_t messageBufferHandle, void * pvRxData, uint8_t xDataLengthBytes, TickType_t xTicksToWait){
	return xMessageBufferReceive(messageBufferHandle, pvRxData, xDataLengthBytes, xTicksToWait);
}

#endif