#include "stm32f10x.h"                  // Device header

CanRxMsg MyCAN_RxMsg;
volatile uint8_t MyCAN_RxFlag;

void MyCAN_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_CAN1EN,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_BS1  = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_Prescaler = 48;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_Init(CAN1,&CAN_InitStructure);
	
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInit(&CAN_FilterInitStructure);

	
}
void MyCAN_Transmit(CanTxMsg *TxMessage)
{
	
	uint8_t TransmitMailbox = CAN_Transmit(CAN1,TxMessage);
	uint32_t TimeOut;
	while(CAN_TransmitStatus(CAN1,TransmitMailbox) != CAN_TxStatus_Ok)
	{
		TimeOut++;
		if(TimeOut<10000)
		{
			break;
		}
	}
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if(CAN_GetFlagStatus(CAN1,CAN_FLAG_FMP0) == SET)
	{
		CAN_Receive(CAN1,CAN_FIFO0,&MyCAN_RxMsg);
		MyCAN_RxFlag = 1;
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}


/* 检查 FIFO 溢出 */
    if(CAN_GetFlagStatus(CAN1, CAN_FLAG_FOV0) == SET)
    {
        CAN_ClearFlag(CAN1, CAN_FLAG_FOV0);
    }
}
