#ifndef __IIC_H
#define __IIC_H

#include "sys.h"

/******************************************************************************
  PocketLab 软件 IIC (IO 模拟) 驱动
  ----------------------------------------------------------------
  原理图连接 (U5 AHT20):
      MCU PB6 --- I2C1_SCL --- R36 0R --- AHT20 SCL
      MCU PB7 --- I2C1_SDA --- R34 0R --- AHT20 SDA
      SDA / SCL 均有 4.7K 上拉电阻到 3.3V (R32/R33)
  ----------------------------------------------------------------
  使用开漏输出 + 外部上拉, 可直接读取从机应答
******************************************************************************/

//-----------------IIC端口定义----------------
#define IIC_SCL_PORT    GPIOB
#define IIC_SCL_PIN     GPIO_Pin_6
#define IIC_SDA_PORT    GPIOB
#define IIC_SDA_PIN     GPIO_Pin_7

#define IIC_SCL_H()     GPIO_SetBits(GPIOB,GPIO_Pin_6)      //SCL输出高
#define IIC_SCL_L()     GPIO_ResetBits(GPIOB,GPIO_Pin_6)    //SCL输出低
#define IIC_SDA_H()     GPIO_SetBits(GPIOB,GPIO_Pin_7)      //SDA输出高(释放)
#define IIC_SDA_L()     GPIO_ResetBits(GPIOB,GPIO_Pin_7)    //SDA输出低
#define IIC_SDA_READ()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//读SDA电平

void IIC_Init(void);//IIC初始化
void IIC_Start(void);//起始信号
void IIC_Stop(void);//停止信号
void IIC_SendByte(u8 dat);//发送一个字节
u8   IIC_ReadByte(u8 ack);//读取一个字节  ack:1-应答 0-非应答
u8   IIC_WaitAck(void);//等待应答  0-应答成功  1-无应答
void IIC_Ack(void);//发送应答
void IIC_NAck(void);//发送非应答

u8 IIC_WriteBytes(u8 dev,u8 *buf,u8 len);//写多字节 返回0成功
u8 IIC_ReadBytes(u8 dev,u8 *buf,u8 len); //读多字节 返回0成功
u8 IIC_WriteByte(u8 dev,u8 dat);         //写单字节 返回0成功
u8 IIC_ReadByteFrom(u8 dev,u8 *dat);     //读单字节 返回0成功
u8 IIC_Probe(u8 dev);                    //探测器件 0-在线

#endif
