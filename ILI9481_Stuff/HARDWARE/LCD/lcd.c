#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"
#include "delay.h"	 
/****************************************Copyright (c)****************************************************
**                                 ������ɽ�ҿƼ����޹�˾     
**                                 ��      ����http://shan-yan.taobao.com
**                                 ΢      ����http://weibo.com/shanyantech
**                                 ��������Ⱥ��204669564
**								   							 Q        Q: 895838470
**--------------File Info---------------------------------------------------------------------------------
** File name:               LCD.c
** Descriptions:            ILI9481 SPI��������
**                          ��Ȩ���У�ʹ���뱣����
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

					 
//������ɫ,������ɫ
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
 

void TFT_SPI_Write_Byte(u8 num)    
{  
	u8 count=0;  
	
	for(count=0;count<8;count++)  
	{ 
		CLR_TCLK();//��������Ч	 
		if(num&0x80)SET_TDIN(); 
		else CLR_TDIN();   
		num<<=1;    
		SET_TCLK();   		
	} 			    
}


void LCD_WR_REG(u8 CMD)	  
{ 
	CLR_TCS(); 
	CLR_DC(); 
	TFT_SPI_Write_Byte(CMD);
	SET_TCS();         
}
  
void LCD_WR_DATA(u8 DATA)	  
{ 	 
	CLR_TCS(); 
	SET_DC(); 
	TFT_SPI_Write_Byte(DATA);
	SET_TCS();   
}

  
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(0X2C);
}	 

	 
//LCD������ʾ
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29); //26��ɫ��ʾ����
}	 


//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);;//�ر���ʾ 
}   


//���ù��λ��
//Xpos:������
//Ypos:������
__inline void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
#if USE_HORIZONTAL
  LCD_WR_REG(0x2A);
	LCD_WR_DATA(Ypos>>8);
  LCD_WR_DATA(0xFF&Ypos);                //�趨X����
  LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);

  LCD_WR_REG(0x002B);
	LCD_WR_DATA(Xpos>>8);
  LCD_WR_DATA(0xFF&Xpos);                //�趨Y����
  LCD_WR_DATA(0x01);
	LCD_WR_DATA(0xdf);
	#else
	LCD_WR_REG(0x2A);
	LCD_WR_DATA(Xpos>>8);
  LCD_WR_DATA(0xFF&Xpos);                //�趨X����
  LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3F);

  LCD_WR_REG(0x2B);
	LCD_WR_DATA(Ypos>>8);
  LCD_WR_DATA(0xFF&Ypos);                //�趨Y����
  LCD_WR_DATA(0x01);
	LCD_WR_DATA(0xdf);
	#endif
}  


//����
//x:0~239
//y:0~319
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);
	LCD_WriteRAM_Prepare();
  LCD_WR_DATA(POINT_COLOR>>8);
	LCD_WR_DATA(POINT_COLOR); 
} 	


//��ʼ��lcd
void LCD_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
    		   
	//ע��,ʱ��ʹ��֮��,��GPIO�Ĳ�������Ч
	//��������֮ǰ,����ʹ��ʱ��.����ʵ���������������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_3);
	
		LCD_WR_REG(0x01);
		delay_ms(50);

		LCD_WR_REG(0x0011);          //??????,0x0010???  ILI9481
		delay_ms(20);
		LCD_WR_REG(0x0013);          //???????? 
  
		LCD_WR_REG(0x00D0);          //??????,??3???
		LCD_WR_DATA(0x0007);         //1.0xVci
		LCD_WR_DATA(0x0040);         //
		LCD_WR_DATA(0x001c);         // 
		LCD_WR_REG(0x00D1);          //Vcom Control ??????
		LCD_WR_DATA(0x00FF);         //
		LCD_WR_DATA(0x00FF);         // 18 
		LCD_WR_DATA(0x00FF);         // 1D  
		LCD_WR_REG(0x00D2);          //????????????,??????
		LCD_WR_DATA(0x0001);         //
		LCD_WR_DATA(0x0011);         //
	
		LCD_WR_REG(0x00C0);          //Panel Driving setting  ??5???
		LCD_WR_DATA(0x0010);         //??????
		LCD_WR_DATA(0x003B);         //????480?
		LCD_WR_DATA(0x0000);         //
		LCD_WR_DATA(0x0002);         //5frames
		LCD_WR_DATA(0x0011);         //
		LCD_WR_REG(0x00C1);          //Timing setting ??3???
		LCD_WR_DATA(0x0010);         //
		LCD_WR_DATA(0x0010);         //LCD_WR_DATA(0x000B);         //
		LCD_WR_DATA(0x0088);         //
		LCD_WR_REG(0x00C5);          //Frame Rate and Inversion Control ??????
		LCD_WR_DATA(0x0001);         //100hz  
		LCD_WR_REG(0x00C8);          //????,??12???
		LCD_WR_DATA(0x0000);LCD_WR_DATA(0x0030);LCD_WR_DATA(0x0036);
		LCD_WR_DATA(0x0045);LCD_WR_DATA(0x0004);LCD_WR_DATA(0x0016);
		LCD_WR_DATA(0x0037);LCD_WR_DATA(0x0075);LCD_WR_DATA(0x0077);
		LCD_WR_DATA(0x0054);LCD_WR_DATA(0x000f);LCD_WR_DATA(0x0000); 
		LCD_WR_REG(0x00E4);LCD_WR_DATA(0x00A0);  
		LCD_WR_REG(0x00F0);LCD_WR_DATA(0x0001); 
		LCD_WR_REG(0x00F3);LCD_WR_DATA(0x0040);
		LCD_WR_DATA(0x000A); 
		LCD_WR_REG(0x00F7);LCD_WR_DATA(0x0080);
		LCD_WR_REG(0x0036);LCD_WR_DATA(0x000A);   //LCD_WR_DATA(0x000a);  //YT24F146CIS--0X00CA,CIB--0X000A 
		LCD_WR_REG(0x003A);LCD_WR_DATA(0x0055);
		LCD_WR_REG(0x002A);
		LCD_WR_DATA(0x0000);
		LCD_WR_DATA(0x0000);
		LCD_WR_DATA(0x0001);
		LCD_WR_DATA(0x003F);

		LCD_WR_REG(0x002B);
		LCD_WR_DATA(0x0000);
		LCD_WR_DATA(0x0000);
		LCD_WR_DATA(0x0001);
		LCD_WR_DATA(0x00df);	 

		delay_ms(50);
		LCD_WR_REG(0x0020); 
	//	LCD_WR_REG(0x0021);  
		LCD_WR_REG(0x0029);   
		LCD_WR_REG(0x002C);   
	
	LCD_LED=1;//��������	 
	LCD_Clear(WHITE);
}  		  
  
//��������
//Color:Ҫ���������ɫ
void LCD_Clear(u16 Color)
{
	u32 index=0;      
	LCD_SetCursor(0x00,0x0000);//���ù��λ�� 
	LCD_WriteRAM_Prepare(); 
	for(index=0;index<153600;index++) 
	{
		LCD_WR_DATA(Color>>8);
		LCD_WR_DATA(Color);
	}
}  


//��ָ�����������ָ����ɫ
//�����С:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
#if USE_HORIZONTAL==1
	xlen=yend-ysta+1;	   
	for(i=xsta;i<=xend;i++)
	{
	 	LCD_SetCursor(i,ysta);      //���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	  
		for(j=0;j<xlen;j++)	 
		{
			LCD_WR_DATA(color>>8);
			LCD_WR_DATA(color);
		}
	}
#else
	xlen=xend-xsta+1;	   
	for(i=ysta;i<=yend;i++)
	{
	 	LCD_SetCursor(xsta,i);      //���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	  
		for(j=0;j<xlen;j++)
		{
			LCD_WR_DATA(color>>8);
			LCD_WR_DATA(color);
		}	    
	}
#endif						  	    
} 


//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//������
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
//��ָ��λ����ʾһ���ַ�
//x:0~234
//y:0~308
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
//��ָ��λ����ʾһ���ַ�
//x:0~234
//y:0~308
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  
#if USE_HORIZONTAL==1
#define MAX_CHAR_POSX 312
#define MAX_CHAR_POSY 232 
#else     
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 312
#endif 
    u8 temp;
    u8 pos,t;
	u16 x0=x;
	u16 colortemp=POINT_COLOR;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	    
	//���ô���		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	if(!mode) //�ǵ��ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_DrawPoint(x,y);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//��һ����     
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//��ʾ2������
//x,y:�������
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~99);	 
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode)
{         	
	u8 t,temp;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
void LCD_ShowString(u16 x,u16 y,const u8 *p)
{         
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear(WHITE);}
        LCD_ShowChar(x,y,*p,16,0);
        x+=8;
        p++;
    }  
}






























