/*
 * uart_debug.c
 *
 * Created: 17/11/2019 19:34:20
 *  Author: maticpi
 */ 
#include "uart_debug.h"

void PrintString(char *str)
{
  printf("%s ",str);
}

void PrintRegValue(char *str, unsigned char reg)
{
  printf("%s0x%02X ",str,reg);  
}