#include "stm32f10x.h"                  // Device header

void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}


void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Initstructure.GPIO_Pin =  GPIO_Pin_5 |GPIO_Pin_7;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	SPI_InitTypeDef SPI_InitStrcture;
	SPI_InitStrcture.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStrcture.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStrcture.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStrcture.SPI_CRCPolynomial = 7;
	SPI_InitStrcture.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStrcture.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStrcture.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStrcture.SPI_Mode = SPI_Mode_Master;
	SPI_InitStrcture.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitStrcture);
	
	SPI_Cmd(SPI1,ENABLE);
}

void MySPI_Start(void)
{
	MySPI_W_SS(0);
}
void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}
uint8_t MySPI_SwapData(uint8_t SendData)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) != SET);
		
	SPI_I2S_SendData(SPI1,SendData);
	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != SET);
	
	return SPI_I2S_ReceiveData(SPI1);
	
}
