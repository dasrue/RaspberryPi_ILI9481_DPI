/* --------------------------Includes ---------------------------------------------*/
#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "Picture.h"
#include "QDTFT_demo.h"
/* ----------------------End of Includes ---------------------------------------------*/

unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};
//���Ʋ��Բ˵�
//2D������ťʾ��
void Redraw_Mainmenu(void)
{
	
}
//�������������
void Num_Test(void)
{
	u8 i=0;
//	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	delay_ms(1000);
//	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%3)*40,32*(i/3)+30,RED,GRAY0,Num[i+1]);
	delay_ms(100);
	}
	
}

//������ʾ����
//16λBMP 40X40 QQͼ��ȡģ����
//Image2LCDȡģѡ������
//ˮƽɨ��
//16λ
//40X40
//������ͼ��ͷ����
//��������
//�Զ�����
//��λ��ǰ
void showimage(const unsigned char *p) //��ʾ40*40 QQͼƬ
{
  	int i,j,k; 
	unsigned char picH,picL; 
//	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"ͼƬ��ʾ����");
	delay_ms(1000);

//	Lcd_Clear(GRAY0);
	for(k=0;k<6;k++)
	{
	   	for(j=0;j<8;j++)
		{	
			Lcd_SetRegion(40*j,40*k,40*j+39,40*k+39);		//��������
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//���ݵ�λ��ǰ
				picH=*(p+i*2+1);				
				Lcd_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}
//���Ժ���
void QDTFT_Test_Demo(void)
{
	Lcd_Init();			
	Lcd_Clear(0xff,0x00,0x00);
	Lcd_Clear(0x00,0xff,0x00);

	delay_ms(1500);

}
