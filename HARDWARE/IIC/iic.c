#include "iic.h"
#include "delay.h"

//IIC总线速率调节: 数值越大速率越慢 (AHT20最高支持400KHz)
#define IIC_DELAY()  delay_us(2)

/******************************************************************************
      函数说明：IIC初始化
      入口数据：无
      返回值：  无
******************************************************************************/
void IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	u8 i;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能B端口时钟

	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN|IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		//开漏输出(外部4.7K上拉)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);

	IIC_SDA_H();//释放总线
	IIC_SCL_H();
	IIC_DELAY();

	//总线恢复: 若SDA被从机拉死, 补9个时钟并发送停止信号
	if(IIC_SDA_READ() == 0)
	{
		for(i=0;i<9;i++)
		{
			IIC_SCL_L();
			IIC_DELAY();
			IIC_SCL_H();
			IIC_DELAY();
		}
	}
	IIC_Stop();
}

/******************************************************************************
      函数说明：起始信号
******************************************************************************/
void IIC_Start(void)
{
	IIC_SDA_H();
	IIC_SCL_H();
	IIC_DELAY();
	IIC_SDA_L();//SCL高电平期间SDA下降沿
	IIC_DELAY();
	IIC_SCL_L();
}

/******************************************************************************
      函数说明：停止信号
******************************************************************************/
void IIC_Stop(void)
{
	IIC_SDA_L();
	IIC_DELAY();
	IIC_SCL_H();
	IIC_DELAY();
	IIC_SDA_H();//SCL高电平期间SDA上升沿
	IIC_DELAY();
}

/******************************************************************************
      函数说明：发送应答
******************************************************************************/
void IIC_Ack(void)
{
	IIC_SDA_L();
	IIC_DELAY();
	IIC_SCL_H();
	IIC_DELAY();
	IIC_SCL_L();
	IIC_DELAY();
	IIC_SDA_H();
}

/******************************************************************************
      函数说明：发送非应答
******************************************************************************/
void IIC_NAck(void)
{
	IIC_SDA_H();
	IIC_DELAY();
	IIC_SCL_H();
	IIC_DELAY();
	IIC_SCL_L();
	IIC_DELAY();
}

/******************************************************************************
      函数说明：等待应答
      返回值：  0-应答成功  1-无应答
******************************************************************************/
u8 IIC_WaitAck(void)
{
	u8 t = 0;

	IIC_SDA_H();//释放SDA, 由从机拉低表示应答
	IIC_DELAY();
	IIC_SCL_H();
	IIC_DELAY();
	while(IIC_SDA_READ())
	{
		if(++t > 250)
		{
			IIC_SCL_L();
			return 1;
		}
	}
	IIC_SCL_L();
	IIC_DELAY();
	return 0;
}

/******************************************************************************
      函数说明：发送一个字节
      入口数据：dat 要发送的数据(高位在前)
******************************************************************************/
void IIC_SendByte(u8 dat)
{
	u8 i;

	for(i=0;i<8;i++)
	{
		if(dat & 0x80) IIC_SDA_H();
		else           IIC_SDA_L();
		IIC_DELAY();
		IIC_SCL_H();
		IIC_DELAY();
		IIC_SCL_L();
		dat <<= 1;
	}
	IIC_DELAY();
}

/******************************************************************************
      函数说明：读取一个字节
      入口数据：ack 1-读完发应答 0-读完发非应答
      返回值：  读到的数据
******************************************************************************/
u8 IIC_ReadByte(u8 ack)
{
	u8 i,dat = 0;

	IIC_SDA_H();//释放SDA, 准备接收
	for(i=0;i<8;i++)
	{
		IIC_DELAY();
		IIC_SCL_H();
		IIC_DELAY();
		dat <<= 1;
		if(IIC_SDA_READ()) dat |= 0x01;
		IIC_SCL_L();
	}

	if(ack) IIC_Ack();
	else    IIC_NAck();

	return dat;
}

/******************************************************************************
      函数说明：向从机写多个字节
      入口数据：dev 从机地址(8位, 含读写位)
                buf 数据缓冲区
                len 数据长度
      返回值：  0-成功  1-地址无应答  2-数据无应答
******************************************************************************/
u8 IIC_WriteBytes(u8 dev,u8 *buf,u8 len)
{
	u8 i;

	IIC_Start();
	IIC_SendByte(dev);
	if(IIC_WaitAck())
	{
		IIC_Stop();
		return 1;
	}
	for(i=0;i<len;i++)
	{
		IIC_SendByte(buf[i]);
		if(IIC_WaitAck())
		{
			IIC_Stop();
			return 2;
		}
	}
	IIC_Stop();

	return 0;
}

/******************************************************************************
      函数说明：从从机读多个字节
      入口数据：dev 从机地址(8位, 含读写位, 读地址)
                buf 数据缓冲区
                len 数据长度
      返回值：  0-成功  1-地址无应答
******************************************************************************/
u8 IIC_ReadBytes(u8 dev,u8 *buf,u8 len)
{
	u8 i;

	IIC_Start();
	IIC_SendByte(dev);
	if(IIC_WaitAck())
	{
		IIC_Stop();
		return 1;
	}
	for(i=0;i<len;i++)
	{
		buf[i] = IIC_ReadByte((i == len-1) ? 0 : 1);
	}
	IIC_Stop();

	return 0;
}

/******************************************************************************
      函数说明：向从机写一个字节
      返回值：  0-成功
******************************************************************************/
u8 IIC_WriteByte(u8 dev,u8 dat)
{
	return IIC_WriteBytes(dev,&dat,1);
}

/******************************************************************************
      函数说明：从从机读一个字节
      返回值：  0-成功
******************************************************************************/
u8 IIC_ReadByteFrom(u8 dev,u8 *dat)
{
	return IIC_ReadBytes(dev,dat,1);
}

/******************************************************************************
      函数说明：探测从机是否在线
      返回值：  0-在线  1-不在线
******************************************************************************/
u8 IIC_Probe(u8 dev)
{
	u8 res;

	IIC_Start();
	IIC_SendByte(dev);
	res = IIC_WaitAck();
	IIC_Stop();

	return res;
}
