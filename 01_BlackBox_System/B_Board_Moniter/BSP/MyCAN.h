#ifndef MYCAN_H
#define MYCAN_H


extern CanRxMsg MyCAN_RxMsg;
extern volatile uint8_t MyCAN_RxFlag;

void MyCAN_Init(void);
void MyCAN_Transmit(CanTxMsg *TxMessage);



#endif
