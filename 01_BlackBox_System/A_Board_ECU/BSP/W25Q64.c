#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "W25Q64_INS.h"

void W25Q64_Init(void)
{
	MySPI_Init();
}
void W25Q64_ReadID(uint8_t *MID,uint16_t *QID)
{
	MySPI_Start();
	MySPI_SwapData(W25Q64_JEDEC_ID);
	*MID = MySPI_SwapData(W25Q64_DUMMY_BYTE);
	*QID = MySPI_SwapData(W25Q64_DUMMY_BYTE);
	*QID <<= 8;
	*QID |= MySPI_SwapData(W25Q64_DUMMY_BYTE);
	MySPI_Stop();
}

void WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapData(W25Q64_WRITE_ENABLE);
	MySPI_Stop();

}
void WaitBusy(void)
{
	MySPI_Start();
	MySPI_SwapData(W25Q64_READ_STATUS_REGISTER_1);
	while((MySPI_SwapData(W25Q64_DUMMY_BYTE) & 0x01) == 0x01);
	MySPI_Stop();
	
}

void PageProgram(uint32_t Address,uint8_t *DataArray,uint16_t Count)
{
	
	uint8_t i;
	WriteEnable();
	MySPI_Start();
	MySPI_SwapData(W25Q64_PAGE_PROGRAM);
	MySPI_SwapData(Address >> 16);
	MySPI_SwapData(Address>>8);
	MySPI_SwapData(Address);
	for(i=0;i<Count;i++)
	{
		MySPI_SwapData(DataArray[i]);
	}
	MySPI_Stop();
	WaitBusy();
}
void SectorErase(uint32_t Address)
{
	WriteEnable();
	MySPI_Start();
	MySPI_SwapData(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapData(Address >> 16);
	MySPI_SwapData(Address>>8);
	MySPI_SwapData(Address);
	MySPI_Stop();
	WaitBusy();
}
void W25Q64_ReadData(uint32_t Address,uint8_t *DataArray,uint16_t Count)
{
	
	uint16_t i;
	MySPI_Start();
	MySPI_SwapData(W25Q64_READ_DATA);
	MySPI_SwapData(Address >> 16);
	MySPI_SwapData(Address>>8);
	MySPI_SwapData(Address);
	for(i=0;i<Count;i++)
	{
		DataArray[i] = MySPI_SwapData(W25Q64_DUMMY_BYTE);
	}
	MySPI_Stop();
	
}
