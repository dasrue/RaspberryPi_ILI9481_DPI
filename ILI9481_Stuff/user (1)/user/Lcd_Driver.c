#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "delay.h"


/***************************************************************************************
STM32����ƽ̨����:
�����壺STM32������
MCU ��STM32_F103_RBT6
���� ��8MHZ
��Ƶ ��72MHZ
����˵��:
//-------------------------------------------------------------------------------------
#define LCD_CTRL   	  	GPIOB		//����TFT���ݶ˿�
#define LCD_LED        	GPIO_Pin_9  //PB9 ������TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10������TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 ������TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12������TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13������TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15������TFT - SDI
//VCC:���Խ�5VҲ���Խ�3.3V
//LED:���Խ�5VҲ���Խ�3.3V����ʹ���������IO����(�ߵ�ƽʹ��)
//GND���ӵ�Դ��
//˵��������Ҫ��������ռ��IO�����Խ�LCD_CS�ӵأ�LCD_LED��3.3V��LCD_RST������Ƭ����λ�ˣ�
//�������ͷ�3������IO
//�ӿڶ�����Lcd_Driver.h�ڶ��壬
//������IO�ӷ������������ʵ�ʽ����޸���ӦIO��ʼ��LCD_GPIO_Init()
//-----------------------------------------------------------------------------------------
���̹���˵����
1.	��ˢ������
2.	Ӣ����ʾ����ʾ��
3.	������ʾ����ʾ��
4.	�����������ʾʾ��
5.	ͼƬ��ʾʾ��
6.	2D�����˵�ʾ��
7.	������֧�ֺ���/�����л�(������USE_HORIZONTAL,���Lcd_Driver.h)
8.	������֧�����ģ��SPI/Ӳ��SPI�л�(������USE_HARDWARE_SPI,���Lcd_Driver.h)
**********************************************************************************************/


//---------------------------------function----------------------------------------------------//

/****************************************************************************
* ��    �ƣ�void LCD_GPIO_Init(void)
* ��    �ܣ�STM32_ģ��SPI���õ���GPIO��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʼ��ģ��SPI���õ�GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
      
}
/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(u8 Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void  SPIv_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //�������
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

/****************************************************************************
* ��    �ƣ�u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* ��    �ܣ�STM32_Ӳ��SPI��дһ���ֽ����ݵײ㺯��
* ��ڲ�����SPIx,Byte
* ���ڲ��������������յ�������
* ˵    ����STM32_Ӳ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
{
	while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);		//�ȴ���������	  
	SPIx->DR=Byte;	 	//����һ��byte   
	while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);//�ȴ�������һ��byte  
	return SPIx->DR;          	     //�����յ�������			
} 

/****************************************************************************
* ��    �ƣ�void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
* ��    �ܣ�����SPI���ٶ�
* ��ڲ�����SPIx,SpeedSet
* ���ڲ�������
* ˵    ����SpeedSet:1,����;0,����;
****************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
{
	SPIx->CR1&=0XFFC7;
	if(SpeedSet==1)//����
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_2;//Fsck=Fpclk/2	
	}
	else//����
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
	}
	SPIx->CR1|=1<<6; //SPI�豸ʹ��
} 

/****************************************************************************
* ��    �ƣ�SPI2_Init(void)
* ��    �ܣ�STM32_SPI2Ӳ�����ó�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����STM32_SPI2Ӳ�����ó�ʼ��
****************************************************************************/
void SPI2_Init(void)	
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	 
	//����SPI2�ܽ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//SPI2����ѡ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);
	   
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	//ʹ��SPI2
	SPI_Cmd(SPI2, ENABLE);   
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteIndex(u8 Index)
* ��    �ܣ���Һ����дһ��8λָ��
* ��ڲ�����Index   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
	unsigned char i=0;
    LCD_CS_CLR;

	LCD_SCL_CLR;
	LCD_SDA_CLR;
	LCD_SCL_SET;

	for(i=8;i>0;i--)
	{
		if(Index&0x80)	
	  LCD_SDA_SET; //�������
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Index<<=1; 
	}
 
   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteData(u8 Data)
* ��    �ܣ���Һ����дһ��8λ����
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ��ڲ�����
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
	unsigned char i=0;
    LCD_CS_CLR;

	LCD_SCL_CLR;
	LCD_SDA_SET;
	LCD_SCL_SET;

	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //�������
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}

   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�void LCD_WriteReg(u8 Index,u16 Data)
* ��    �ܣ�д�Ĵ�������
* ��ڲ�����Index,Data
* ���ڲ�������
* ˵    ����������Ϊ��Ϻ�������Index��ַ�ļĴ���д��Dataֵ
****************************************************************************/
void LCD_WriteReg(u8 Index,u16 Data)
{
	Lcd_WriteIndex(Index);
  	Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* ��    �ƣ�void Lcd_WriteData_16Bit(u16 Data)
* ��    �ܣ���Һ����дһ��16λ����
* ��ڲ�����Data
* ���ڲ�������
* ˵    �����������ָ����ַд��һ��16λ����
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{	
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);	
}

/****************************************************************************
* ��    �ƣ�void Lcd_Reset(void)
* ��    �ܣ�Һ��Ӳ��λ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��ǰ��ִ��һ�θ�λ����
****************************************************************************/
void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(50);
}
/****************************************************************************
* ��    �ƣ�void Lcd_Init(void)
* ��    �ܣ�Һ����ʼ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{	
	
	LCD_GPIO_Init();//ʹ��ģ��SPI

	Lcd_Reset(); //Reset before LCD Init.
		 
	 Lcd_WriteIndex(0x11);
	 	delay_ms(20);

	 Lcd_WriteIndex(0xD0);
	 Lcd_WriteData(0x07);
	 Lcd_WriteData(0x41);
	 Lcd_WriteData(0x1d);

	 Lcd_WriteIndex(0xD1);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x2b);
	 Lcd_WriteData(0x1f);

	 Lcd_WriteIndex(0xD2);
	 Lcd_WriteData(0x01);
	 Lcd_WriteData(0x11);

	 Lcd_WriteIndex(0xC0);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x3B);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x02);
	 Lcd_WriteData(0x11);
	 Lcd_WriteData(0x00);

	 Lcd_WriteIndex(0xC5);
	 Lcd_WriteData(0x03);

	 Lcd_WriteIndex(0xC6);  //interface control
	 Lcd_WriteData(0x80);

	 Lcd_WriteIndex(0xC8);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x14);
	 Lcd_WriteData(0x33);
	 Lcd_WriteData(0x10);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x16);
	 Lcd_WriteData(0x44);
	 Lcd_WriteData(0x36);
	 Lcd_WriteData(0x77);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x0f);
	 Lcd_WriteData(0x00);

	 	 	 
	 Lcd_WriteIndex(0xF3);
	 Lcd_WriteData(0x40);
	 Lcd_WriteData(0x0A);

	 Lcd_WriteIndex(0xF0);
	 Lcd_WriteData(0x08);

	 Lcd_WriteIndex(0xF6);
	 Lcd_WriteData(0x84);

	 Lcd_WriteIndex(0xF7);
	 Lcd_WriteData(0x80);

	 Lcd_WriteIndex(0x0C);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x55);

	 Lcd_WriteIndex(0xB4);
	 Lcd_WriteData(0x00);

	 Lcd_WriteIndex(0xB3);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x01);
	 Lcd_WriteData(0x06);
	 Lcd_WriteData(0x01);

	 Lcd_WriteIndex(0x36);
	 Lcd_WriteData(0x48);

	 Lcd_WriteIndex(0x3a);
	 Lcd_WriteData(0x66);

	 Lcd_WriteIndex(0x20);
//	 Lcd_WriteIndex(0x21);

	 Lcd_WriteIndex(0x2A);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x01);
	 Lcd_WriteData(0x3F);

	 Lcd_WriteIndex(0x2B);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x01);
	 Lcd_WriteData(0xdf);

	 delay_ms(120);

	 Lcd_WriteIndex(0x29);
	 delay_ms(120);

	 Lcd_WriteIndex(0x2C);
}



/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData_16Bit(Xpos);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData_16Bit(Ypos);
	Lcd_WriteIndex(0x2c);	
} 
/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
//������ʾ����
void Lcd_SetRegion(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData_16Bit(xStar);
	Lcd_WriteData_16Bit(xEnd);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData_16Bit(yStar);
	Lcd_WriteData_16Bit(yEnd);
	Lcd_WriteIndex(0x2c);
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�����xy�������ɫ����
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);

}    

/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(u8 colorH,u8 colorz,u8 colorL)               
{	
   unsigned int i,m;
	 Lcd_WriteIndex(0x2A);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x01);
	 Lcd_WriteData(0x3F);

	 Lcd_WriteIndex(0x2B);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x00);
	 Lcd_WriteData(0x01);
	 Lcd_WriteData(0xe0);

	 Lcd_WriteIndex(0x2C);

	 for (i=0;i<480;i++){
		for (m=0;m<320;m++){
		Lcd_WriteData(colorH);
		Lcd_WriteData(colorz);
		Lcd_WriteData(colorL);

			}  
	 }
}


