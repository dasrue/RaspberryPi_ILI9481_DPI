
#include <intrins.h>
#include <reg51.h>
#include <absacc.h>

sbit	CSB	=	P3^3;
sbit	RESETB	=	P3^4;
sbit	SDA	=	P3^5;
sbit	SCL	=	P3^6;


sbit	KEY =	P3^2;


void LCD_INIT_ILI9481(void);
void LCD_WRITE_DATA_dis(char colorH,char colorL);
void LCD_TEST_SingleColor_16BPP(char colorH,char colorL);
void LCD_TEST_SingleColor(char colorH,char colorz,char colorL);
void LCD_WRITE_CMD(char cmd1);
void LCD_WRITE_DATA(char dataa);
void LCD_TEST_barColor();
void HD_reset(void);
void Keypress(void);

void delayms(int value);
void delayus(int value1);
unsigned char code barcolor[];


unsigned char code barcolor[]={
0Xff,0X00,0X00,0X00,0Xff,0X00,0X00,0X00,0Xff,0Xff,0Xff,0Xff,0X00,0X00,0X00,0Xff,
0Xff,0X00,0X00,0Xff,0Xff,0Xff,0X00,0Xff,
};									  
void main(){

        HD_reset();
        delayms(50);
		LCD_INIT_ILI9481();
	while(1)
	{
/*       LCD_TEST_SingleColor(0xff,0x00,0x00); //display red

       LCD_TEST_SingleColor(0x00,0xff,0x00);  //display blue

	   LCD_TEST_SingleColor(0x00,0x00,0xff);  //display blue

       LCD_TEST_SingleColor(0xff,0xff,0x00);  //display yellow

	   LCD_TEST_SingleColor(0x00,0xff,0xff);  //display 
*/
//	   LCD_TEST_SingleColor_16BPP(0xf8,0x00);
// Keypress();
//	   LCD_TEST_SingleColor_16BPP(0x07,0xE0);
//Keypress();
//	   LCD_TEST_SingleColor_16BPP(0x00,0x1F);
//Keypress();
  LCD_TEST_barColor();
//Keypress();
	}
}

void HD_reset(){
	RESETB=1;
	delayms(1);	
	RESETB=0;
	delayms(10);
	RESETB=1;
	delayms(100);
	}

   void LCD_INIT_ILI9481(void)
	{

	 LCD_WRITE_CMD(0x11);
	 	delayms(20);

	 LCD_WRITE_CMD(0xD0);
	 LCD_WRITE_DATA(0x07);
	 LCD_WRITE_DATA(0x41);
	 LCD_WRITE_DATA(0x1d);

	 LCD_WRITE_CMD(0xD1);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x2b);
	 LCD_WRITE_DATA(0x1f);

	 LCD_WRITE_CMD(0xD2);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0x11);

	 LCD_WRITE_CMD(0xC0);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x3B);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x02);
	 LCD_WRITE_DATA(0x11);
	 LCD_WRITE_DATA(0x00);

	 LCD_WRITE_CMD(0xC5);
	 LCD_WRITE_DATA(0x03);

	 LCD_WRITE_CMD(0xC6);  //interface control
	 LCD_WRITE_DATA(0x80);

	 LCD_WRITE_CMD(0xC8);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x14);
	 LCD_WRITE_DATA(0x33);
	 LCD_WRITE_DATA(0x10);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x16);
	 LCD_WRITE_DATA(0x44);
	 LCD_WRITE_DATA(0x36);
	 LCD_WRITE_DATA(0x77);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x0f);
	 LCD_WRITE_DATA(0x00);

	 	 	 
	 LCD_WRITE_CMD(0xF3);
	 LCD_WRITE_DATA(0x40);
	 LCD_WRITE_DATA(0x0A);

	 LCD_WRITE_CMD(0xF0);
	 LCD_WRITE_DATA(0x08);

	 LCD_WRITE_CMD(0xF6);
	 LCD_WRITE_DATA(0x84);

	 LCD_WRITE_CMD(0xF7);
	 LCD_WRITE_DATA(0x80);

	 LCD_WRITE_CMD(0x0C);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x55);

	 LCD_WRITE_CMD(0xB4);
	 LCD_WRITE_DATA(0x00);

	 LCD_WRITE_CMD(0xB3);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0x06);
	 LCD_WRITE_DATA(0x01);

	 LCD_WRITE_CMD(0x36);
	 LCD_WRITE_DATA(0x48);

	 LCD_WRITE_CMD(0x3a);
	 LCD_WRITE_DATA(0x66);

	 LCD_WRITE_CMD(0x20);
//	 LCD_WRITE_CMD(0x21);

	 LCD_WRITE_CMD(0x2A);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0x3F);

	 LCD_WRITE_CMD(0x2B);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0xdf);

	 delayms(120);

	 LCD_WRITE_CMD(0x29);
	 delayms(120);

	 LCD_WRITE_CMD(0x2C);

	} 


//-------------------------------9 bit chansfer------------
void LCD_WRITE_CMD(char cmd1)
{
  	unsigned char nub;

	CSB=0;

	SCL=0;				 
	SDA=0;
	SCL=1;
	for(nub=0;nub<8;nub++)
	{
		SCL=0;			
			if((cmd1&0x80)==0x80)
			{
				SDA=1;
			}
			else
			{
				SDA=0;
			}
			cmd1=(cmd1<<1);
		SCL=1;

	}
	CSB=1;

}

void LCD_WRITE_DATA(char dataa)
{
  	unsigned char nub;

	CSB=0;	

	SCL=0;		 
	SDA=1;
	SCL=1;
	for(nub=0;nub<8;nub++)
	{
		SCL=0;			
			if((dataa&0x80)==0x80)
			{
				SDA=1;
			}
			else
			{
				SDA=0;
			}
			dataa=(dataa<<1);
		SCL=1;
	}
	CSB=1;

}

//--------------------16bp218bp--------------
void LCD_WRITE_DATA_dis(char colorH,char colorL)
{
	LCD_WRITE_DATA(colorH&0xf8);
	LCD_WRITE_DATA(((colorH&0x07)<<5)+((colorL&0xe0)>>3));
	LCD_WRITE_DATA((colorL&0x1f)<<3);
}

void LCD_TEST_SingleColor(char colorH,char colorz,char colorL){
     int i,j;

	 LCD_WRITE_CMD(0x2A);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0x3F);

	 LCD_WRITE_CMD(0x2B);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0xe0);

	 LCD_WRITE_CMD(0x2C);

	 for (i=0;i<480;i++){
		for (j=0;j<320;j++){
		LCD_WRITE_DATA(colorH);
		LCD_WRITE_DATA(colorz);
		LCD_WRITE_DATA(colorL);

			}
		}
	}
 //##########################ºì£¬ÂÌ£¬À¶£¬»Æ£¬³È£¬ºÚ£¬°×¡£×Ï############
void LCD_TEST_barColor(){
     int i,j,k=0,p;

	 LCD_WRITE_CMD(0x2A);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0x3F);

	 LCD_WRITE_CMD(0x2B);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0xe0);

	 LCD_WRITE_CMD(0x2C);

	 for (p=0;p<8;p++)
	 {
	 	for (i=0;i<60;i++){
			for (j=0;j<320;j++){
			LCD_WRITE_DATA(barcolor[k]);
			LCD_WRITE_DATA(barcolor[k+1]);
			LCD_WRITE_DATA(barcolor[k+2]);
			}
		}
		k++;
		k++;
		k++;
	  }
	}

void LCD_TEST_SingleColor_16BPP(char colorH,char colorL){
     int i,j;

	 LCD_WRITE_CMD(0x2A);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0x3F);

	 LCD_WRITE_CMD(0x2B);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x00);
	 LCD_WRITE_DATA(0x01);
	 LCD_WRITE_DATA(0xe0);

	 LCD_WRITE_CMD(0x2C);

	 for (i=0;i<480;i++){
		for (j=0;j<320;j++){
		LCD_WRITE_DATA_dis(colorH,colorL);

			}
		}
	}

void Keypress()
	{
	while(KEY)
	delayms(10);
	}

void delayms(int value)
	{
	while (value){
		delayus(99);
		value--;
		}
	}

void delayus(int value1)
	{
	while (value1)

	value1--;
	}
