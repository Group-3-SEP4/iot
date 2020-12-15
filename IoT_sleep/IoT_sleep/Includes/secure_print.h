#ifndef SECURE_PRINT_H
#define SECURE_PRINT_H

#include <semphr.h>

void s_print_create(SemaphoreHandle_t mutex);
void s_print(char* level, char* tag, char* msg, ...);

#endif
