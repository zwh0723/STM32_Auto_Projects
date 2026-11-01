#include "stm32f10x.h"
#include "Delay.h"
#include "MyCAN.h"
#include "serial.h" // 你的串口初始化头文件
#include <stdio.h>



// 串口重定向（如果你之前的工程里有，直接复制过来）
int fputc(int ch, FILE *f) {
    Serial_Sendbyte(ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}

int main(void)
{
    Serial_Init(); // 初始化串口
    MyCAN_Init();  // 初始化 CAN（开启中断）
    
    printf("=== B 板 CAN 接收节点已启动 ===\r\n");

    while(1)
    {
//		printf("Flag:%d\r\n",MyCAN_RxFlag);
        if (MyCAN_RxFlag == 1)
        {
				// 判断是不是 0x200 传感器数据
            if (MyCAN_RxMsg.StdId == 0x200)
            {
                int16_t rx_ax = (int16_t)((MyCAN_RxMsg.Data[0] << 8) | MyCAN_RxMsg.Data[1]);
                printf("收到 0x200 报文 | AX = %d\r\n", rx_ax);
            }
            // 判断是不是 0x500 碰撞报警
            else if (MyCAN_RxMsg.StdId == 0x500)
            {
				int16_t rx_ax = (int16_t)((MyCAN_RxMsg.Data[0] << 8) | MyCAN_RxMsg.Data[1]);
                printf("⚠️ 警告：收到 0x500 碰撞报警报文！| AX = %d\r\n", rx_ax);
				
            }
            
            MyCAN_RxFlag = 0; // 清除标志位
        }
    }
}