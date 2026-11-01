#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"
#include "MyCAN.h"
#include "MPU6050.h"
#include "MySPI.h"
#include "W25Q64.h"

int16_t ax,ay,az;
int16_t gx,gy,gz;
uint8_t CAN_Data[6];

int main(void)
{
    OLED_Init();
	MyCAN_Init();
	MPU_Init();
	MySPI_Init();
	W25Q64_Init();
	
	int16_t saved_ax = 0;
	
	W25Q64_ReadData(0x000000,(uint8_t*)&saved_ax,2);
	
	OLED_ShowHexNum(1,11,saved_ax,4);


    while(1)
    {  
		MPU_GetData(&ax,&ay,&az,&gx,&gy,&gz);
		
		OLED_ShowString(1,1,"Ax");
		OLED_ShowSignedNum(1,4,ax,5);
		OLED_ShowString(2,1,"Ay");
		OLED_ShowSignedNum(2,4,ay,5);
		OLED_ShowString(3,1,"Az");
		OLED_ShowSignedNum(3,4,az,5);
		
		
		
		CAN_Data[0] = (ax >> 8) & 0xFF;
		CAN_Data[1] = (ax & 0xFF);
		CAN_Data[2] = (ay >> 8) & 0xFF;
		CAN_Data[3] = (ay & 0xFF);
		CAN_Data[4] = (az >> 8) & 0xFF;
		CAN_Data[5] = (az & 0xFF);
		
		CanTxMsg TxMsg;
		TxMsg.DLC = 6;
		TxMsg.IDE = CAN_Id_Standard;
		TxMsg.RTR = CAN_RTR_Data;
		TxMsg.StdId = 0x200;

		for(uint8_t i = 0;i<6;i++)
		{
			TxMsg.Data[i] = CAN_Data[i];
		}
		
		if(CAN_Transmit(CAN1,&TxMsg) == CAN_TxStatus_Ok)
		{
//			chenggon
		}
		if(MyCAN_RxFlag == 1)
		{
			OLED_ShowString(4,1,"Rx");
			OLED_ShowHexNum(4,4,MyCAN_RxMsg.StdId,3);
			if(MyCAN_RxMsg.StdId == 0x200)
			{
				int16_t Rxax = (int16_t)((MyCAN_RxMsg.Data[0] << 8) | MyCAN_RxMsg.Data[1]);
				int16_t Rxay = (int16_t)((MyCAN_RxMsg.Data[2] << 8) | MyCAN_RxMsg.Data[3]);
				int16_t Rxaz = (int16_t)((MyCAN_RxMsg.Data[4] << 8) | MyCAN_RxMsg.Data[5]);
				
				OLED_ShowHexNum(2,1,Rxax,5);
				OLED_ShowHexNum(2,7,Rxay,5);
				OLED_ShowHexNum(3,1,Rxaz,5);
			}	
			MyCAN_RxFlag = 0;
		}
		static uint8_t already_saved = 0;
		if((ax > 400 || ax< -8000) && !already_saved)
		{
			already_saved = 1;
			OLED_ShowString(1,1,"saving");
			
			CanTxMsg TxMsg;
			TxMsg.DLC = 1;
			TxMsg.IDE = CAN_Id_Standard;
			TxMsg.RTR = CAN_RTR_Data;
			TxMsg.StdId = 0x500;
			TxMsg.Data[0] = 0x01;
			
			SectorErase(0x000000);
			PageProgram(0x000000,(uint8_t*)&ax,2);
			CAN_Transmit(CAN1,&TxMsg);
			OLED_ShowString(4,1,"save  ");	
			OLED_ShowHexNum(4,8,ax,4);			
		}
		if((ax > 10000 || ax< -10000) && !already_saved)
		{
			already_saved = 0;
		}
		Delay_ms(100);
	}
	
}
