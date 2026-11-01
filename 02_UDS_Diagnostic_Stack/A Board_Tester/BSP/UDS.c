#include "stm32f10x.h"                  // Device header
#include "string.h"

void isotp_send(uint8_t *Data,uint8_t len)
{
	CanTxMsg tx;
		tx.StdId = 0x7E8;
		tx.IDE = CAN_Id_Standard;
		tx.RTR = CAN_RTR_Data;
	if(len<=7)
	{
		tx.DLC = len+1;
		tx.Data[0] =len;
		for(uint8_t i = 0;i<len;i++)
		{
			tx.Data[i+1] = Data[i];
		}
		CAN_Transmit(CAN1,&tx);
		return;
	}
	tx.DLC = 8;
	tx.Data[0] = 0x10 | ((len >> 8)& 0x0F);
	tx.Data[1] = len & 0xFF;
	for(uint8_t i = 0;i<6;i++)
	{
		tx.Data[i+2] = Data[i];
	}
	CAN_Transmit(CAN1,&tx);
	
	uint16_t sent = 6;
	uint8_t sn = 1;
	while(sent<len)
	{
		uint8_t n = ((len-sent) > 7) ? 7 :(len-sent);
		
		tx.DLC = n + 1;
		tx.Data[0] = 0x20 | (sn & 0x0F);
		for(uint8_t i = 0;i<n;i++)
		{
			tx.Data[i+1] = Data[sent + i];
		}
		CAN_Transmit(CAN1,&tx);
		
		sent += n;
		sn = (sn+1) & 0x0F;

	}
	
}

void Send_nrc(uint8_t svc,uint8_t nrc)
{
	uint8_t resp[3];
	resp[0] = 0x7F;
	resp[1] = svc;
	resp[2] = nrc;
	isotp_send(resp,3);
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
			if(len<2) 
			{
				Send_nrc(0x10,0x13);
				break;
			}
		
		current_session = req[1];
		security_unlocked = 0;
		resp[0] = 0x50;
		resp[1] = req[1];
		resp[2] = 0x00; resp[3] = 0x32;
		resp[4] = 0x01; resp[5] = 0xF4;
		resp_len = 6;
		break;
		case 0x22:
			if(len <3)
			{
				Send_nrc(0x22,0x13);
				return;
			}
			uint16_t did = (req[1]<<8 | req[2]);
			
			if(did == 0xF186)
			{
				resp[0] = 0x62;
				resp[1] = 0xF1;
				resp[2] = 0x86; 
				resp[3] = current_session;
				resp_len = 4;
			}else if(did == 0xF187)
			{
				resp[0] = 0x62;
				resp[1] = 0xF1;
				resp[2] = 0x87;
				memcpy(&resp[3],"P001",4);
				resp_len = 7;
			}
			else if(did == 0xF190)
			{
				uint8_t vin_data[20];
				vin_data[0] = 0x62; vin_data[1] = 0xF1; vin_data[2] = 0x90;
				memcpy(&vin_data[3],"L12345678901234567",17);
				isotp_send(vin_data ,20);
				return;
			}
			{
				Send_nrc(0x22,0x31);
				return;
			}
			break;
		case 0x27:
			if(len <2)
			{
				Send_nrc(0x27,0x13);
				return;
			}
			if(req[1] == 0x01)
			{
				resp[0] = 0x67;
				resp[1] = 0x01;
				resp[2] = 0x12; resp[3] = 0x34;
				resp[4] = 0x56; resp[5] = 0x78;
				resp_len = 6;
			}
			else if(req[1] == 0x02)
			{
				if(len<6) {Send_nrc(0x27,0x13); return;}
				uint32_t key = (req[2] << 24) | req[3] << 16 | req[4] << 8 | req[5];
				if(key == 0x12345678 ^ 0x5A5A5A5A)
				{
					security_unlocked = 1;
					resp[0] = 0x67; resp[1] = 0x02;
					resp_len = 2;
				}else 
				{
				Send_nrc(0x27,0x35);
				return;
				}
				break;
			}
			case 0x2E:                              /* 写数据 */
				if (!security_unlocked) { Send_nrc(0x2E, 0x33); break; }   /* 未解锁 */
				if (current_session != 0x03) { Send_nrc(0x2E, 0x22); break; }
				if (len < 3) { Send_nrc(0x2E, 0x13); break; }
			
				/* 存起来（存到变量或 Flash） */
				resp[0] = 0x6E;
				resp[1] = req[1]; resp[2] = req[2];
				resp_len = 3;
				break;
			
			default:
				Send_nrc(svc, 0x11);                /* 服务不支持 */
				return;
		
	}
	isotp_send(resp,resp_len);
			
}