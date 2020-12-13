/*
 *  Author: Jesper 273961
 */ 
#include <stdio.h>
#include <string.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <stdarg.h>
#include <string.h>
#include "definitions.h"
#include "secure_print.h"

static SemaphoreHandle_t mutex_print;

void s_print_create(void) {
	mutex_print = xSemaphoreCreateMutex();
}


void s_print(char* level, char* tag, char* msg, ...) {
	if (NULL != mutex_print && (DEF_PRINT_TO_TERMINAL == true || strcmp("INFO", level) != 0)){ 
		if (xSemaphoreTake(mutex_print, DEF_WAIT_DEFAULT) == pdTRUE) { 
			char buffer[512];
			va_list args;
			va_start(args, msg);
			vsprintf(buffer, msg, args);
			va_end(args);
			
			printf("%-7s --- %-25s : %s \n", level, tag, buffer);
		}
	} 
	 
}

