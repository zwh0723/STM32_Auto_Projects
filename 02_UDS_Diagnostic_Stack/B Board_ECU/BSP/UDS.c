#include "stm32f10x.h"                  // Device header
#include "string.h"

void UDS_Send(uint8_t *Data,uint8_t len)
{
	if(len>7) return;
	CanTxMsg tx;
	tx.StdId = 0x7E8;
	tx.IDE = CAN_Id_Standard;
	tx.RTR = CAN_RTR_Data;
	tx.DLC = len+1;
	tx.Data[0] =len;
	for(uint8_t i = 0;i<len;i++)
	{
		tx.Data[i+1] = Data[i];
	}
	CAN_Transmit(CAN1,&tx);
}

void Send_nrc(uint8_t svc,uint8_t nrc)
{
	uint8_t resp[3];
	resp[0] = 0x7F;
	resp[1] = svc;
	resp[2] = nrc;
	UDS_Send(resp,3);
}

uint8_t current_session = 0x01;
uint8_t security_unlocked = 0;

void UDS_handle(uint8_t *req,uint8_t len)
{
	uint8_t svc = req[0];
	uint8_t resp[16];
	uint8_t resp_len = 0;
	
	switch(svc)
	{
		case 0x10:
			if(len<2) Send_nrc(0x10,0x13);break;
		
		current_session = req[1];
		security_unlocked = 0;
		resp[0] = 0x50;
		resp[1] = req[1];
		resp[2] = 0x00; resp[3] = 0x32;
		resp[4] = 0x01; resp[5] = 0xF4;
		resp_len = 6;
		break;
		
		UDS_Send(resp,resp_len);
			
	}
}
