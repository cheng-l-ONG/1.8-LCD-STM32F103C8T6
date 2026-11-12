#include "aht20.h"
#include "iic.h"
#include "delay.h"

#define AHT20_CMD_CAL      0xBE   //初始化/校准命令
#define AHT20_CMD_TRIGGER  0xAC   //触发测量命令
#define AHT20_CMD_STATUS   0x71   //读状态字命令

/******************************************************************************
      函数说明：读AHT20状态字
      入口数据：sta 状态字指针
      返回值：  0-成功
******************************************************************************/
u8 AHT20_ReadStatus(u8 *sta)
{
	IIC_Start();
	IIC_SendByte(AHT20_ADDR_W);
	if(IIC_WaitAck())
	{
		IIC_Stop();
		return 1;
	}
	IIC_SendByte(AHT20_CMD_STATUS);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(AHT20_ADDR_R);
	if(IIC_WaitAck())
	{
		IIC_Stop();
		return 1;
	}
	*sta = IIC_ReadByte(0);//最后一个字节回非应答
	IIC_Stop();

	return 0;
}

/******************************************************************************
      函数说明：发送初始化(校准)命令 0xBE 0x08 0x00
******************************************************************************/
static u8 AHT20_Calibrate(void)
{
	u8 buf[3];

	buf[0] = AHT20_CMD_CAL;
	buf[1] = 0x08;
	buf[2] = 0x00;

	return IIC_WriteBytes(AHT20_ADDR_W,buf,3);
}

/******************************************************************************
      函数说明：AHT20初始化
                上电后至少等待100ms, 读取状态字判断是否已校准,
                未校准则发送 0xBE 0x08 0x00 进行校准
      返回值：  0-成功  1-器件无应答  2-校准失败
******************************************************************************/
u8 AHT20_Init(void)
{
	u8 sta = 0;
	u8 i;

	IIC_Init();
	delay_ms(120);//上电稳定时间(手册要求>=100ms)

	if(AHT20_IsOnline()) return 1;//器件无应答

	//最多尝试3次校准
	for(i=0;i<3;i++)
	{
		if(AHT20_ReadStatus(&sta)) return 1;
		if((sta & (AHT20_STATUS_CAL|AHT20_STATUS_BUSY)) == AHT20_STATUS_CAL)
		{
			break;//已校准且空闲
		}
		AHT20_Calibrate();
		delay_ms(50);
	}

	if(AHT20_ReadStatus(&sta)) return 1;
	if((sta & AHT20_STATUS_CAL) == 0) return 2;//仍未校准

	return 0;
}

/******************************************************************************
      函数说明：触发一次测量并读取温湿度
      入口数据：temperature 温度值(摄氏度)指针
                humidity    湿度值(相对湿度%)指针
      返回值：  0-成功  1-发送命令失败  2-读数据失败  3-传感器忙
******************************************************************************/
u8 AHT20_Read(float *temperature,float *humidity)
{
	u8 buf[7];
	u8 cmd[3];
	u8 retry;
	u32 raw_h,raw_t;

	cmd[0] = AHT20_CMD_TRIGGER;
	cmd[1] = 0x33;
	cmd[2] = 0x00;

	//1.触发测量
	if(IIC_WriteBytes(AHT20_ADDR_W,cmd,3)) return 1;

	//2.等待转换完成(手册要求 >=80ms)
	delay_ms(80);

	//3.读取状态字 + 6字节数据 (共7字节)
	for(retry=0;retry<5;retry++)
	{
		if(IIC_ReadBytes(AHT20_ADDR_R,buf,7)) return 2;
		if((buf[0] & AHT20_STATUS_BUSY) == 0) break;//bit7=0 转换完成
		delay_ms(20);
	}
	if(retry >= 5) return 3;

	//4.数据拼接: 20bit湿度 + 20bit温度
	raw_h = ((u32)buf[1] << 12) | ((u32)buf[2] << 4) | (buf[3] >> 4);
	raw_t = (((u32)(buf[3] & 0x0F)) << 16) | ((u32)buf[4] << 8) | buf[5];

	//5.转换为实际物理量 (2^20 = 1048576)
	*humidity    = (float)raw_h * 100.0f / 1048576.0f;
	*temperature = (float)raw_t * 200.0f / 1048576.0f - 50.0f;

	return 0;
}

/******************************************************************************
      函数说明：检测器件是否在线
      返回值：  0-在线  1-不在线
******************************************************************************/
u8 AHT20_IsOnline(void)
{
	return IIC_Probe(AHT20_ADDR_W);
}
