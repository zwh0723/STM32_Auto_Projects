# STM32 汽车电子学习项目

基于 STM32F103C8T6 的两个实践项目：
1. **车载数据记录仪** —— CAN 双节点通信 + 急刹触发 + 掉电存储
2. **UDS 诊断协议栈** —— ISO-TP 传输 + 诊断服务实现

## 硬件

| 器件 | 用途 | 接口 |
|---|---|---|
| STM32F103C8T6 ×2 | 双节点 | — |
| SN65HVD230 ×2 | CAN 收发器（含 120Ω 终端） | CAN |
| MPU6050 | 三轴加速度 | I2C |
| W25Q64 | 外部 Flash 存储 | SPI |
| OLED 0.96" | 状态显示 | 软件 I2C |
| CH340 | 串口调试 | USART |
| USB 逻辑分析仪 | 时序抓取 | — |

## 目录

```
01_BlackBox_System/        车载数据记录仪
02_UDS_Diagnostic_Stack/   UDS 诊断协议栈
```

## 项目一：车载数据记录仪

双节点 CAN 台架。A 板采集 MPU6050 三轴数据经 CAN 上报，超阈值时发报警帧
并写入 W25Q64；B 板作为监控终端接收并串口输出，用于验证总线连通性。

**关键参数**
- CAN 波特率 500kbps：Prescaler=4, BS1=15tq, BS2=2tq, SJW=1tq，采样点 88.9%
- 状态帧 ID `0x200`（DLC=6，三轴 int16 大端拆分）
- 报警帧 ID `0x500`（DLC=1）
- 触发阈值 3000（约 0.18g，便于手动演示；真实急刹约 0.4–0.6g）

**实现要点**
- 数据打包：3 个 int16 拆成 6 字节，接收端按高字节在前重组
- 中断接收：FIFO0 中断内只搬数据 + 置标志，解析与显示放主循环
- 掉电存储：SectorErase + PageProgram，开机回读上次数据

**排错记录**
- CAN 发送失败时观察到 `CAN_Transmit` 返回 `NoMailBox(4)`：
  3 个发送邮箱被失败帧占满，硬件无限重试
- TEC 累积至 Bus-Off 的过程中发现计数器被 ABOM 自动清零，
  关闭 ABOM 后才观察到完整 80→255→BusOff 过程

→ 详细说明见 [01_BlackBox_System/README.md](01_BlackBox_System/README.md)

## 项目二：UDS 诊断协议栈

两块板分别模拟诊断仪（Tester）与 ECU，实现 ISO-TP 传输层与 UDS 诊断服务。

**报文约定**
- 请求：CAN ID `0x7E0`；响应：CAN ID `0x7E8`
- 单帧 PCI：`0x0N`（N=数据长度）
- 首帧 `0x1H 0xLL` + 6 字节；连续帧 `0x2S` + 7 字节；流控 `0x3S BS STmin`

**已实现服务**

| SID | 服务 | 说明 |
|---|---|---|
| 0x10 | 会话控制 | 默认/编程/扩展会话 |
| 0x22 | 读数据 | 按 DID 返回，如 F186 当前会话、F187 零件号 |
| 0x27 | 安全访问 | seed/key，校验失败返回 NRC 0x35 |
| 0x2E | 写数据 | 需扩展会话 + 安全解锁，否则 NRC 0x22 / 0x33 |

**否定响应**：`7F + SID + NRC`，覆盖 0x11（服务不支持）、0x13（长度错误）、
0x31（DID 越界）、0x33（未解锁）

**验证**
- 多帧收发：读 VIN 返回 20 字节，走 FF(6) + CF(7) + CF(7)
- 跳号测试：连续帧序号错乱时接收端丢弃整包
- 流控测试：BS=1 时发送端等流控帧后再续发

→ 详细说明见 [02_UDS_Diagnostic_Stack/README.md](02_UDS_Diagnostic_Stack/README.md)

## 技能清单

- **MCU**：STM32F103 标准外设库；GPIO/TIM(PWM/输入捕获)/ADC/DMA/EXTI/
  USART/I2C/SPI/Flash/看门狗
- **通信**：CAN（位定时、掩码滤波、中断收发、错误计数）、
  ISO-TP（单帧/多帧/流控）、UDS（0x10/0x22/0x27/0x2E）
- **调试**：Keil MDK、串口日志、万用表、USB 逻辑分析仪（UART/I2C 时序）

## 备注

- ISO-TP 多帧部分参考公开实现，跳号与流控容错测试已自行验证
- I2C 采用软件实现：STM32F103 硬件 I2C 在实测中出现 START 标志
  （SB）不置位，排查引脚电平、接线、复位、时钟速率后确认为该型号
  外设已知问题