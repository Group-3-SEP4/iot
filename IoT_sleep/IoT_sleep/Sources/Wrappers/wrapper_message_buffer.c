#include "wrapper_message_buffer.h"

size_t _xMessageBufferSend( MessageBufferHandle_t xMessageBuffer,
const void *pvTxData,
size_t xDataLengthBytes,
TickType_t xTicksToWait ){
	return xMessageBufferSend(
	xMessageBuffer
	, pvTxData
	, xDataLengthBytes
	, xTicksToWait
	);
}

size_t _xMessageBufferReceive( MessageBufferHandle_t xMessageBuffer,
void *pvRxData,
size_t xBufferLengthBytes,
TickType_t xTicksToWait ){
	return xMessageBufferReceive(
	xMessageBuffer
	, pvRxData
	, xBufferLengthBytes
	, xTicksToWait );
	
}