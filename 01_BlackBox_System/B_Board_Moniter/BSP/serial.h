#ifndef _USART_H
#define _USART_H
#include <stdio.h>

void usart_Init(void);
void Serial_Sendbyte(uint8_t byte);
void Serial_SendArray(uint8_t *Array,uint16_t length);
void Serial_SendString(char *string);
uint8_t Serial_GetRxData(void);
uint8_t Serial_GetRxFlag(void);
void USART1_IRQHandler(void);

#endif