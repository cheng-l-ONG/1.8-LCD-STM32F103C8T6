#ifndef __AHT20_H
#define __AHT20_H

#include "sys.h"

/******************************************************************************
  AHT20 温湿度传感器驱动 (基于软件 IIC)
  ----------------------------------------------------------------
  器件地址 : 0x38 (7位)  --> 写 0x70 / 读 0x71
  接线     : SCL-PB6  SDA-PB7 (PocketLab 原理图)
  ----------------------------------------------------------------
  返回值约定 : 0-成功  非0-失败
******************************************************************************/

#define AHT20_ADDR_W     0x70   //写地址
#define AHT20_ADDR_R     0x71   //读地址

//状态字位定义
#define AHT20_STATUS_BUSY   0x80   //bit7 忙标志
#define AHT20_STATUS_CAL    0x08   //bit3 已校准标志

u8 AHT20_Init(void);                            //初始化(上电校准)
u8 AHT20_ReadStatus(u8 *sta);                   //读状态字
u8 AHT20_Read(float *temperature,float *humidity);//读温度(℃)和湿度(%RH)
u8 AHT20_IsOnline(void);                        //检测器件是否在线 0-在线

#endif
