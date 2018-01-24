#ifndef __TSENSOR_H
#define __TSENSOR_H	
#include "stm32f10x.h"

/*
#define T_ADC_CH0  0  //通道0
#define T_ADC_CH1  1  //通道1
#define T_ADC_CH2  2  //通道2
#define T_ADC_CH3  3  //通道3
#define T_TEMP_CH  16 //温度传感器通道
*/
	   
u16  T_Get_Temp(void);  //取得温度值
void T_Adc_Init(void); //ADC通道初始化
u16  T_Get_Adc(u8 ch); //获得某个通道值  	  
#endif 
