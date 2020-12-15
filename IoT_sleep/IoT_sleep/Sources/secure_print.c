#include <stdio.h>
#include <string.h>
#include <ATMEGA_FreeRTOS.h>
#include <stdarg.h>
#include <string.h>
#include "definitions.h"
#include "wrapper_semaphore.h"
#include "secure_print.h"

static SemaphoreHandle_t mutex_print;

void s_print_create(SemaphoreHandle_t mutex) {
	mutex_print = mutex;
}


void s_print(char* level, char* tag, char* msg, ...) {
	if (NULL != mutex_print && (DEF_PRINT_TO_TERMINAL == true || strcmp("INFO", level) != 0)){
		if (_xSemaphoreTake(mutex_print, DEF_WAIT_DEFAULT) == pdTRUE) {
			static char buffer[128]; // automatically cuts the message if bigger than 128 chars.
			va_list args;
			va_start(args, msg);
			vsprintf(buffer, msg, args);
			va_end(args);
			printf("\n%s \n", buffer);
			_xSemaphoreGive(mutex_print);
		}
	}
	
}