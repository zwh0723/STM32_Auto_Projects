#ifndef __MPU6050_H
#define __MPU6050_H

void MPU_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU_ReadReg(uint8_t RegAddress);
void MPU_Init(void);
void MPU_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ);
uint8_t GetID(void);
void I2C_Checkevent(I2C_TypeDef* I2Cx,uint32_t Event);

#endif
