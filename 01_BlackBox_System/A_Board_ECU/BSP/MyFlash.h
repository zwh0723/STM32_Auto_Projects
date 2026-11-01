#ifndef _MYFLASH_H
#define _MYFLASH_H

uint32_t MyFlash_ReadWord(uint32_t Address);
uint16_t MyFlash_ReadHalfWord(uint32_t Address);
uint8_t MyFlash_ReadByte(uint32_t Address);
void MYFLASH_EraseAllPages(void);
void MYFLASH_ErasePage(uint32_t PageAddress);
void MYFLASH_ProgramWord(uint32_t Address,uint32_t Data);
void MYFLASH_ProgramHalfWord(uint32_t Address,uint16_t Data);

#endif