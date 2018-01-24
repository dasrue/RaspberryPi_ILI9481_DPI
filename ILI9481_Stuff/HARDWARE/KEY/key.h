#ifndef __KEY_H
#define __KEY_H	 
 

#define KEY0 PCin(5)   
#define KEY1 PCin(4)	
#define KEY2 PCin(11)	
#define KEY3 PCin(12)	

void KEY_Init(void);//IO初始化
u8   KEY_Scan(void);  //按键扫描函数					    
#endif
