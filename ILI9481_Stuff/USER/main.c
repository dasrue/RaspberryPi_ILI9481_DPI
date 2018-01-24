#include "led.h"
#include "delay.h"
#include "sys.h"
#include "key.h"
#include "usart.h"
#include "lcd.h"
#include "spi.h"
#include "flash.h"
/****************************************Copyright (c)****************************************************
**                                 深圳市山岩科技有限公司     
**                                 淘      宝：http://shan-yan.taobao.com
**                                 微      博：http://weibo.com/shanyantech
**                                 技术交流群：204669564
**								   							 Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            ILI9481 SPI驱动程序
**                          版权所有，使用请保留！
**--------------------------------------------------------------------------------------------------------
** Created by:              shanyan
** Created date:            2013-12-19
** Version:                 v0.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:          
** Version:               
** Descriptions:            
**
*********************************************************************************************************/


 int main(void)
 {
	u8 x=0;
	 
	SystemInit();
	delay_init(72);	     //延时初始化
	NVIC_Configuration();
 	uart_init(9600);
 	LED_Init();
 	KEY_Init();
	LCD_Init();

	POINT_COLOR=RED;//设置字体为蓝色	   
	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(30,50,"ShanYan TECH ^_^");	
		LCD_ShowString(30,70,"3.5' SPI TFTLCD TEST");	    					 
	  x++;
		if(x==12)x=0;
		LED0=!LED0;					 
		delay_ms(1000);	
		delay_ms(1000);	
	}
 }

