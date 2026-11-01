#ifndef W25Q64_H
#define W25Q64_H

void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID,uint16_t *QID);
void PageProgram(uint32_t Address,uint8_t *DataArray,uint16_t Count);
void SectorErase(uint32_t Address);
void W25Q64_ReadData(uint32_t Address,uint8_t *DataArray,uint16_t Count);

#endif