/*####COPYRIGHTBEGIN####
 -------------------------------------------
Copyright (c) 2017 Brad L.

THE FOLLOWING LICENSE APPLIES ONLY TO FILES IN THE esp8266-BACnet/src FOLDER.
SEE OTHER FILES FOR LICENSE TERMS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 -------------------------------------------
####COPYRIGHTEND####*/

/*
--------------------------------------------
This program will bitbang the 9bit SPI required to setup the ILI9481 to use the
parallel RGB interface (AKA DPI, TTL, RGB or any number of things). This uses
the http://www.airspayce.com/mikem/bcm2835/ GPIO library.
--------------------------------------------
*/

#include <stdio.h>
#include <stdint.h>
#include "bcm2835.h"

#define SPI_HOLDTIMEUS	5

const uint8_t gpio_pin_cs	= 22;	// Define pins here as a const. TODO: variable pins based on cmdline arguments?
const uint8_t gpio_pin_clk	= 23;
const uint8_t gpio_pin_mosi	= 24;

unsigned char barcolor[]={
0Xff,0X00,0X00,0X00,0Xff,0X00,0X00,0X00,0Xff,0Xff,0Xff,0Xff,0X00,0X00,0X00,0Xff,
0Xff,0X00,0X00,0Xff,0Xff,0Xff,0X00,0Xff,
}; // Taken from "c51source.c" given by aliexpress seller

// Initialise the GPIO for bitbanging the SPI
void SPI_Init() {
	bcm2835_gpio_fsel(gpio_pin_cs, BCM2835_GPIO_FSEL_OUTP);		
	bcm2835_gpio_set(gpio_pin_cs);	// CS is active low, so set CS high to start
	bcm2835_gpio_fsel(gpio_pin_clk, BCM2835_GPIO_FSEL_OUTP);	
	bcm2835_gpio_set(gpio_pin_clk);	// CLK should idle high according to ILI9481 datasheet						
	bcm2835_gpio_fsel(gpio_pin_mosi, BCM2835_GPIO_FSEL_OUTP);	
	bcm2835_gpio_clr(gpio_pin_mosi);
}

// Assert the CS line to the ILI9481 (pull it low)
void SPI_CSAssert() {
	bcm2835_gpio_clr(gpio_pin_cs);
}

// Deassert the CS line to the ILI9481 (push it high)
void SPI_CSDeassert() {
	bcm2835_gpio_set(gpio_pin_cs);
}

// Send a value, anywhere up to 32 bits to the bitbanged SPI port. CS should be asserted first.
void SPI_Send(uint8_t bitLen, uint32_t value) {
	if(bitLen > 32)
		bitLen = 32;	// Chop bitLen to 32
	
	for(int8_t i = bitLen - 1; i >= 0; i--) {
		bcm2835_delayMicroseconds(SPI_HOLDTIMEUS);
		bcm2835_gpio_clr(gpio_pin_clk);		// Pull the clock low.
		bcm2835_gpio_write(gpio_pin_mosi, (value >> i) & 1);	// Setup the data line
		bcm2835_delayMicroseconds(SPI_HOLDTIMEUS);
		bcm2835_gpio_set(gpio_pin_clk);		// Push the clock high. This will clock the value into the ILI9841
	}
}

// Send a data byte to the ILI9481
void ILI_SendDataByte(uint8_t dataByte) {
	SPI_CSAssert();
	uint16_t newValue = dataByte | (1 << 8);	// Setup the 9 bit byte that is sent to the ILI9481
	SPI_Send(9, newValue);	// Send these 9 bits
	SPI_CSDeassert();
}

// Send a CMD byte to the ILI9481
void ILI_SendCMDByte(uint8_t cmdByte) {
	SPI_CSAssert();
	uint16_t newValue = cmdByte;	// Setup the 9 bit byte that is sent to the ILI9481
	SPI_Send(9, newValue);	// Send these 9 bits
	SPI_CSDeassert();
}

void LCD_TEST_barColor(){ // Taken from "c51source.c" given by aliexpress seller
	int i,j,k=0,p;

	ILI_SendCMDByte(0x2A);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x01);
	ILI_SendDataByte(0x3F);

	ILI_SendCMDByte(0x2B);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x01);
	ILI_SendDataByte(0xe0);

	ILI_SendCMDByte(0x2C);

	for (p=0;p<8;p++){
		for (i=0;i<60;i++){
			for (j=0;j<320;j++){
				ILI_SendDataByte(barcolor[k]);
				ILI_SendDataByte(barcolor[k+1]);
				ILI_SendDataByte(barcolor[k+2]);
			}
		}
	k++;
	k++;
	k++;
	}
}

int main(int argc, char* argv[]) {
	
	if(!bcm2835_init())
		return 1;
	SPI_Init();

	
	// Taken from "c51source.c" given by aliexpress seller

	ILI_SendCMDByte(0x11);
	bcm2835_delay(20);

	ILI_SendCMDByte(0xD0);
	ILI_SendDataByte(0x07);
	ILI_SendDataByte(0x41);
	ILI_SendDataByte(0x1d);

	ILI_SendCMDByte(0xD1);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x2b);
	ILI_SendDataByte(0x1f);

	ILI_SendCMDByte(0xD2);
	ILI_SendDataByte(0x01);
	ILI_SendDataByte(0x11);

	ILI_SendCMDByte(0xC0);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x3B);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x02);
	ILI_SendDataByte(0x11);
	ILI_SendDataByte(0x00);

	ILI_SendCMDByte(0xC5);
	ILI_SendDataByte(0x03);

	ILI_SendCMDByte(0xC6);  //interface control
	ILI_SendDataByte(0x80);

	ILI_SendCMDByte(0xC8);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x14);
	ILI_SendDataByte(0x33);
	ILI_SendDataByte(0x10);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x16);
	ILI_SendDataByte(0x44);
	ILI_SendDataByte(0x36);
	ILI_SendDataByte(0x77);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x0f);
	ILI_SendDataByte(0x00);

		 
	ILI_SendCMDByte(0xF3);
	ILI_SendDataByte(0x40);
	ILI_SendDataByte(0x0A);

	ILI_SendCMDByte(0xF0);
	ILI_SendDataByte(0x08);

	ILI_SendCMDByte(0xF6);
	ILI_SendDataByte(0x84);

	ILI_SendCMDByte(0xF7);
	ILI_SendDataByte(0x80);

	ILI_SendCMDByte(0x0C);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x55);

	ILI_SendCMDByte(0xB4);
	ILI_SendDataByte(0x00);

	ILI_SendCMDByte(0xB3);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x01);
	ILI_SendDataByte(0x06);
	ILI_SendDataByte(0x01);

	ILI_SendCMDByte(0x36);
	ILI_SendDataByte(0x48);

	ILI_SendCMDByte(0x3a);
	ILI_SendDataByte(0x66);

	ILI_SendCMDByte(0x20);
	//	 ILI_SendCMDByte(0x21);

	ILI_SendCMDByte(0x2A);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x01);
	ILI_SendDataByte(0x3F);

	ILI_SendCMDByte(0x2B);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x00);
	ILI_SendDataByte(0x01);
	ILI_SendDataByte(0xdf);

	bcm2835_delay(120);

	ILI_SendCMDByte(0x29);
	bcm2835_delay(120);

	ILI_SendCMDByte(0x2C);

	printf("Display initialised!\r\n");
	LCD_TEST_barColor();
	bcm2835_close();
	return 0;
}
