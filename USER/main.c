#include "delay.h"
#include "sys.h"
#include "lcd_init.h"
#include "lcd.h"
#include "iic.h"
#include "aht20.h"

/******************************************************************************
      函数说明：在指定位置显示一个浮点数(两位小数, 支持负数)
      入口数据：x,y  起点坐标
                num  要显示的数
                fc   前景色  bc 背景色
                sizey 字体高度(12/16/24/32)
      返回值：  无
******************************************************************************/
static void LCD_ShowFloat(u16 x,u16 y,float num,u16 fc,u16 bc,u8 sizey)
{
	u8 sizex = sizey/2;
	u8 buf[6];
	u8 n = 0,i;
	u16 ip,dp;

	if(num < 0)
	{
		LCD_ShowChar(x,y,'-',fc,bc,sizey,0);
		x += sizex;
		num = -num;
	}

	ip = (u16)num;
	dp = (u16)((num - (float)ip) * 100.0f + 0.5f);
	if(dp >= 100)
	{
		ip++;
		dp -= 100;
	}

	if(ip == 0)
	{
		buf[n++] = '0';
	}
	else
	{
		while(ip > 0)
		{
			buf[n++] = (u8)(ip % 10) + '0';
			ip /= 10;
		}
	}
	for(i = n;i > 0;i--)
	{
		LCD_ShowChar(x,y,buf[i-1],fc,bc,sizey,0);
		x += sizex;
	}
	LCD_ShowChar(x,y,'.',fc,bc,sizey,0);
	x += sizex;
	LCD_ShowChar(x,y,(u8)(dp/10)+'0',fc,bc,sizey,0);
	x += sizex;
	LCD_ShowChar(x,y,(u8)(dp%10)+'0',fc,bc,sizey,0);
}

/******************************************************************************
      函数说明：显示界面固定部分
******************************************************************************/
static void LCD_ShowUI(void)
{
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);

	LCD_ShowString(8,4,"PocketLab",WHITE,BLACK,16,0);
	LCD_ShowString(8,22,"AHT20 Sensor",BLUE,BLACK,16,0);
	LCD_DrawLine(0,42,LCD_W-1,42,BLUE);//分割线

	LCD_ShowString(8,46,"Temp:",CYAN,BLACK,16,0);
	LCD_ShowString(8,92,"Humi:",CYAN,BLACK,16,0);

	LCD_ShowString(110,64,"C",WHITE,BLACK,24,0);
	LCD_ShowString(110,110,"%",WHITE,BLACK,24,0);
}

int main(void)
{
	float temp = 0.0f,humi = 0.0f;
	u8 res;
	u8 last_state = 0xFF;

	delay_init();//延时函数初始化
	AHT20_Init();//软件IIC + AHT20初始化
	LCD_Init();//LCD初始化

	LCD_ShowUI();
	LCD_ShowString(8,142,"Sensor init...",GRAY,BLACK,16,0);

	res = AHT20_IsOnline();
	LCD_Fill(8,142,120,158,BLACK);
	if(res == 0) LCD_ShowString(8,142,"Sensor OK",GREEN,BLACK,16,0);
	else         LCD_ShowString(8,142,"Sensor ERR",RED,BLACK,16,0);

	while(1)
	{
		res = AHT20_Read(&temp,&humi);

		if(res == 0)
		{
			LCD_Fill(6,64,104,88,BLACK);   //清空温度显示区
			LCD_ShowFloat(8,64,temp,WHITE,BLACK,24);

			LCD_Fill(6,110,104,134,BLACK);//清空湿度显示区
			LCD_ShowFloat(8,110,humi,WHITE,BLACK,24);

			if(last_state != 0)
			{
				last_state = 0;
				LCD_Fill(8,142,120,158,BLACK);
				LCD_ShowString(8,142,"Sensor OK",GREEN,BLACK,16,0);
			}
		}
		else
		{
			if(last_state != res)
			{
				last_state = res;
				LCD_Fill(8,142,120,158,BLACK);
				LCD_ShowString(8,142,"Read Error",RED,BLACK,16,0);
			}
		}

		delay_ms(1000);//1秒刷新一次
	}
}
