#include "stm32f10x.h"                  // Device header
#include "UDS.h"
#include <string.h>
#include <stdio.h>
#include "serial.h"

typedef struct {
    uint8_t  buf[256];      /* 重组缓冲 */
    uint16_t total_len;     /* 总长度*/
    uint16_t received;      /* 已收字节数 */
    uint8_t  next_sn;       /* 期望的下一个序号 */
    uint8_t  active;        /* 是否在处理多帧 */
} isotp_rx_t;

static isotp_rx_t rx;

// B板专用：接收并处理多帧
void isotp_on_can_frame(uint8_t *data, uint8_t dlc) 
{
    uint8_t pci = data[0];
    
    switch(pci >> 4) 
    {
        case 0x0: // 单帧（比如以前的 50 03）
            printf("单帧: ");
            for(int i=1; i<= (pci & 0x0F); i++) printf("%02X ", data[i]);
            printf("\r\n");
            break;
            
        case 0x1: // 首帧（VIN码第一部分）
            rx.total_len = ((pci & 0x0F) << 8) | data[1];
            memcpy(rx.buf, &data[2], 6);
            rx.received = 6;
            rx.next_sn = 1;
            rx.active = 1;
            
            // 🔥 B 板主动给 A 板回流控帧（允许发送）
            CanTxMsg fc;
            fc.StdId = 0x7E0;
            fc.IDE = CAN_Id_Standard;
            fc.RTR = CAN_RTR_Data;
            fc.DLC = 3;
            fc.Data[0] = 0x30; // 继续发
            fc.Data[1] = 0x00; // 不限制
            fc.Data[2] = 0x00; // 不间隔
            CAN_Transmit(CAN1, &fc);
            break;
            
        case 0x2: // 连续帧（VIN码后续）
            if (!rx.active) break;
            if ((pci & 0x0F) != rx.next_sn) {
                rx.active = 0; // 序号错，丢弃
                printf("丢包啦！\r\n");
                break;
            }
            
            uint16_t remain = rx.total_len - rx.received;
            uint8_t n = (remain > 7) ? 7 : remain;
            
            memcpy(&rx.buf[rx.received], &data[1], n);
            rx.received += n;
            rx.next_sn = (rx.next_sn + 1) & 0x0F;
            
            if (rx.received >= rx.total_len) {
                rx.active = 0;
                printf("收到完整VIN码: ");
                for(int i=3; i<rx.total_len; i++) { 
                    printf("%c", rx.buf[i]);
                }
                printf("\r\n");
            }
            break;
    }
}