#ifndef UDS_H
#define UDS_H
#include "stm32f10x.h"                  // Device header

extern uint8_t current_session;
extern uint8_t security_unlocked;
void UDS_Send(uint8_t *Data,uint8_t len);
void Send_nrc(uint8_t svc,uint8_t nrc);
void UDS_handle(uint8_t *req,uint8_t len);
#endif
