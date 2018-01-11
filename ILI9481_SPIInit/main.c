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
	uint16_t newValue = dataByte | (1 << 8);	// Setup the 9 bit byte that is sent to the ILI9481
	SPI_Send(9, newValue);	// Send these 9 bits
}

// Send a CMD byte to the ILI9481
void ILI_SendCMDByte(uint8_t cmdByte) {
	uint16_t newValue = cmdByte;	// Setup the 9 bit byte that is sent to the ILI9481
	SPI_Send(9, newValue);	// Send these 9 bits
}


int main(int argc, char* argv[]) {
	
	if(!bcm2835_init())
		return 1;
	SPI_Init();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0x01);	// Soft reset 
	SPI_CSDeassert();
	bcm2835_delay(200);		// Wait 200ms afterwards (datasheet says 10 frames = 166ms at 60fps. Use 200 to be safe)
	
	SPI_CSAssert();
	ILI_SendCMDByte(0x11);	// Enable display
	SPI_CSDeassert();
	bcm2835_delay(6);		// Datasheet says to wait for at least 5ms afterwards
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xD0);	// Set power options
	ILI_SendDataByte(0x07);	// Vci1 is max (2.5V)
	ILI_SendDataByte(0x42);	// Vlout3 enabled, DDVDH = Vci1 * 2 (5V), VCL = -Vci1 (-2.5V), VGH = Vci1 * 6 (15V), VGL = -Vci1 * 3 (-7.5V)
	ILI_SendDataByte(0x17);	// Vci = internal 2.5V, VREG1OUT = 4.75V
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xD1);	// Set VCOM options
	ILI_SendDataByte(0x00);	// Use this register for VCM settings
	ILI_SendDataByte(0x0B);	// VCOMH = VREG1OUT x 0.740 = 3.515V
	ILI_SendDataByte(0x11);	// VCOM amplitude = VREG1OUT x 1.04 = 4.94V
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xD2);	// Set Power_Setting options for normal mode
	ILI_SendDataByte(0x01);	// Gamma Amp = 1.00, Src Amp = 1.00 (Max power)
	ILI_SendDataByte(0x02);	// Stepup Circuit 1 Freq = Fosc, Stepup Circuit 2 Freq = Fosc/64
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xC0);	// Set Panel Driving setting
	ILI_SendDataByte(0x10);	// Invert image?
	ILI_SendDataByte(0x3B);	// Set panel drive lines to 3B, = 8*(0x3B + 1) = 480 Lines
	ILI_SendDataByte(0x00);	// Set scan start position to 0.
	ILI_SendDataByte(0x00);	// Normal VCOM opertion in non-display area (probably irrelevant for this)
	ILI_SendDataByte(0x10);	// Drive pixels inverse in non-display area.
	ILI_SendDataByte(0x01);	// Normal scan mode, Interval scan set to 3 frames (50ms)
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xC5);	// Set framerate
	ILI_SendDataByte(0x02);	// 85Hz
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xC8);	// Set gamma
	ILI_SendDataByte(0x00);	// Values stolen from https://github.com/Bodmer/TFT_HX8357_Due/blob/master/TFT_HX8357_Due.cpp line 454
	ILI_SendDataByte(0x32);
	ILI_SendDataByte(0x36);
	ILI_SendDataByte(0x45);
	ILI_SendDataByte(0x06);
	ILI_SendDataByte(0x16);
	ILI_SendDataByte(0x37);
	ILI_SendDataByte(0x75);
	ILI_SendDataByte(0x77);
	ILI_SendDataByte(0x54);
	ILI_SendDataByte(0x0C);
	ILI_SendDataByte(0x00);
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0x3A);	// Set pixel format
	ILI_SendDataByte(0x66);	// 18Bit
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0xB4);	// Set "Display mode and frame memory write setting"
	ILI_SendDataByte(0x11);	// Use DPI interface for both display and RAM.
	SPI_CSDeassert();
	
	SPI_CSAssert();
	ILI_SendCMDByte(0x29);	// Set display on
	SPI_CSDeassert();
	
	printf("Display initialised!\r\n");
	bcm2835_close();
	return 0;
}
