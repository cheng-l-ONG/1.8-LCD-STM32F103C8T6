# PocketLab 温湿度显示（STM32 + ST7735S + AHT20）

基于 STM32F103/W55MH32 的 PocketLab 开发板，1.8 寸 ST7735S 屏显示 AHT20 温湿度。

## 硬件连接
| 信号 | 引脚 |
|---|---|
| LCD SCL | PC11 |
| LCD SDA | PC12 |
| LCD RES | PD6 |
| LCD DC  | PD5 |
| LCD CS  | PC10 |
| LCD BLK | PA8 |
| AHT20 SCL | PB6 |
| AHT20 SDA | PB7 |

## 编译
Keil MDK5 打开 `USER/LCD.uvprojx` 编译下载。

## 目录结构
- `HARDWARE/IIC` 软件 IIC 驱动
- `HARDWARE/AHT20` AHT20 温湿度驱动
- `HARDWARE/LCD` 中景园 LCD 驱动
- `USER/main.c` 主程序

## 致谢 / 版权
LCD 驱动来自中景园电子，仅供学习交流；STM32 标准外设库版权归 STMicroelectronics 所有。
