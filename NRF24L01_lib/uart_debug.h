/*
 * uart_debug.h
 *
 * Created: 17/11/2019 19:33:56
 *  Author: maticpi
 */ 


#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include <stdio.h>

void PrintString(char *str);
void PrintRegValue(char *str, unsigned char reg);

#endif /* UART_DEBUG_H_ */