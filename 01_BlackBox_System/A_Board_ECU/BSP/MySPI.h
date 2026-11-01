#ifndef MYSPI_H
#define MYSPI_H

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapData(uint8_t SendData);

#endif
