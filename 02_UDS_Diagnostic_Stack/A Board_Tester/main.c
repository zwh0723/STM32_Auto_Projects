#include "stm32f10x.h"
#include "delay.h"
#include "OLED.h"
#include "Key.h"
#include "MyCAN.h"
#include "UDS.h"
#include <string.h>



int main(void)
{
    OLED_Init();
	MyCAN_Init();
	Key_Init();
	OLED_ShowString(1,1,"kkk");
	
	
	
    while(1)
    {  
		if(MyCAN_RxFlag == 1)
		{
			if(MyCAN_RxMsg.StdId == 0x7E0)
			{
				if(MyCAN_RxMsg.Data[0] >>4 == 0x0)
				{
					uint8_t len = MyCAN_RxMsg.Data[0] &0x0F;
					
					uint8_t uds_req[8];
					for(uint8_t i = 0;i<len;i++)
					{
						uds_req[i] = MyCAN_RxMsg.Data[i+1];
					}
					UDS_handle(uds_req,len);
				}
//				OLED_ShowHexNum(1,1,uds_req[0],2);
//				OLED_ShowHexNum(2,1,uds_req[1],2);
//				OLED_ShowHexNum(3,1,uds_req[2],2);
//				OLED_ShowHexNum(4,1,len,2);
			}
			MyCAN_RxFlag = 0;
		}
	}
	
}
