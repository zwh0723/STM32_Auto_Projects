#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"
#include "Key.h"
#include "MyCAN.h"
#include "UDS.h"
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "ISOTP.h"

int fputc(int ch, FILE *f) {
    Serial_Sendbyte(ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
void UDS_Send_Request(uint8_t *Data,uint8_t len)
{
	if(len>7) return;
	CanTxMsg tx;
	tx.StdId = 0x7E0;
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


int main(void)
{	
	serial_Init();
    OLED_Init();
	MyCAN_Init();
	Key_Init();
	

    while(1)
    {  
		uint8_t keynum = Key_GetNum();
		if(keynum == 2)
		{
			uint8_t req[] = {0x10,0x03};
			UDS_Send_Request(req,2);
			
			Delay_ms(300);
		}
		if(keynum == 1)
		{
			uint8_t req[] = {0x22,0xF1,0x90};
			UDS_Send_Request(req,3);
		}
		if(MyCAN_RxFlag == 1)
		{
			if(MyCAN_RxMsg.StdId == 0x7E8)
			{
				isotp_on_can_frame(MyCAN_RxMsg.Data,MyCAN_RxMsg.DLC);
//				uint8_t len = MyCAN_RxMsg.Data[0] &0x0F;
//				
//				for(uint8_t i = 0;i<len;i++)
//				{
//					printf("%02X",MyCAN_RxMsg.Data[i+1]);

//				}
//				printf("\r\n");
			}
			MyCAN_RxFlag = 0;
		}
	}
	
}
