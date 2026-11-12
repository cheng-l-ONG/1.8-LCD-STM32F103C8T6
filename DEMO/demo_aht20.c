#include "delay.h"
#include "sys.h"
#include "lcd_init.h"
#include "lcd.h"
#include "iic.h"
#include "aht20.h"

void demo_aht20(void){
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