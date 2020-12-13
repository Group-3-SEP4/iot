/*
 *  Author: Jesper 273961
 */ 


#ifndef SECURE_PRINT_H
#define SECURE_PRINT_H

void s_print_create(SemaphoreHandle_t mutex);
void s_print(char* level, char* tag, char* msg, ...);



#endif /* SECURE_PRINT_H_ */