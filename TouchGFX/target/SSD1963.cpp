/*
 * SSD1963.cpp
 *
 *  Created on: Jan 14, 2021
 *      Author: archer
 */
#include "main.h"
#include <TouchGFXHAL.hpp>
#include <touchgfx/hal/HAL.hpp>
#include "ssd1963.hpp"

static uint8_t isTransmittingData = 0;

void SSD1963_Send_Cmd(uint16_t cmd)
{
	HAL_GPIO_WritePin(LCD_DnC_GPIO_Port, LCD_DnC_Pin, GPIO_PIN_RESET);
	SSD1963_CMD = cmd;
}
void SSD1963_Send_Dat(uint16_t data)
{
	HAL_GPIO_WritePin(LCD_DnC_GPIO_Port, LCD_DnC_Pin, GPIO_PIN_SET);
	SSD1963_DAT = data;
}
bool SSD1963_Rd_Dat(uint16_t data)
{
	HAL_GPIO_WritePin(LCD_DnC_GPIO_Port, LCD_DnC_Pin, GPIO_PIN_SET);
	uint16_t read = SSD1963_DAT;

	if(data != read)
	{
		return false;
	}
	return true;
}

void SSD1963_SetDisplayWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t bWrite)
{
	// setup column start and end
	SSD1963_Send_Cmd(SSD1963_COLUMN_ADDR);
	SSD1963_Send_Dat(x1 >> 8);
	SSD1963_Send_Dat(x1 & 0x00FF);
	SSD1963_Send_Dat(x2 >> 8);
	SSD1963_Send_Dat(x2 & 0x00FF);

	// setup page start and end
	SSD1963_Send_Cmd(SSD1963_PAGE_ADDR);
	SSD1963_Send_Dat(y1 >> 8);
	SSD1963_Send_Dat(y1 & 0x00FF);
	SSD1963_Send_Dat(y2 >> 8);
	SSD1963_Send_Dat(y2 & 0x00FF);

	// tell SSD1963 to get ready for memory write to frame buffer
	if(bWrite)
		SSD1963_Send_Cmd(SSD1963_WR_MEM);
	else
		SSD1963_Send_Cmd(SSD1963_RD_MEM);
}


void SSD1963_Clear(uint16_t data, uint8_t bCheck)
{
    unsigned int l=480,w;

    SSD1963_SetDisplayWindow(0, 0, 799, 479, 1);	// setup for writes
	while(l--)
	{
	    for( w = 0 ;w < 800; w++)
		{
	    	SSD1963_Send_Dat(data);
		}
	}
	if(bCheck)
	{
		l = 480;
	    SSD1963_SetDisplayWindow(0, 0, 799, 479, 1);	// setup for writes
		while(l--)
		{
		    for( w = 0 ;w < 800; w++)
			{
		    	if(!SSD1963_Rd_Dat(data))
		    		printf("LCD data error: l=%d, w=%d", 480 - (l+1), w);
			}
		}
	}
}

void SSD1963_Test()
{
    unsigned int l=480,w;
    int i = 0;

    SSD1963_SetDisplayWindow(0,799,0,479, 1);
    while(1)
    {
    	l = 480;
		while(l--)
		{
			for( w = 0 ;w < 800; w++)
			{
				SSD1963_Send_Dat(i++);
			}
		}
    }
}

void SSD1963_Init(void)
{
	//1. Power up the system platform and assert the RESET# signal (‘L’ state) for a minimum of 100us to reset the controller.
	SSD1963_RST_SET
	HAL_Delay (100);
	SSD1963_RST_RESET
	HAL_Delay (120);
	SSD1963_RST_SET
	HAL_Delay(150);
	/*	2. Configure SSD1961’s PLL frequency
	 VCO = Input clock x (M + 1)
	 PLL frequency  = VCO / (N + 1)
	 * Note :
	 1.  250MHz < VCO < 800MHz
	 PLL frequency < 110MHz
	 2.  For a 10MHz input clock to obtain 100MHz PLL frequency, user cannot program M = 19 and N = 1.  The
	 closet setting in this situation is setting M=29 and N=2, where 10 x 30 / 3 = 100MHz.
	 3.  Before PLL is locked, SSD1961/2/3 is operating at input clock frequency (e.g. 10MHz), registers
	 programming cannot be set faster than half of the input clock frequency (5M words/s in this example).
	 Example to program SSD1961 with M = 29, N = 2, VCO = 10M x 30 = 300 MHz, PLL frequency = 300M / 3 = 100
	 MHz
	 ******************************/
	// while(true)
	SSD1963_Send_Cmd(SSD1963_RESET);
	SSD1963_Send_Cmd(SSD1963_DISPLAY_ON);
	SSD1963_Send_Cmd(SSD1963_SET_FREQ);		//set frequency
	SSD1963_Send_Dat(0x001D); 			 	// presceller(M=29)
	SSD1963_Send_Dat(0x0002); 	 			//multiplier(N=2)
	SSD1963_Send_Dat(0x0054);  				//on-off multiplier and presceller
	HAL_Delay(2);

	//3. Turn on the PLL
	SSD1963_Send_Cmd(SSD1963_PLL_ON);		// 0xE0);
	SSD1963_Send_Dat(0x01);
	HAL_Delay(10); 							// Wait for 10us to let the PLL stable and read the PLL lock status bit.

	SSD1963_Send_Cmd(SSD1963_PLL_ON);
	//READ COMMAND “0xE4);   (Bit 2 = 1 if PLL locked)
	SSD1963_Send_Dat(0x03); 				// 5. Switch the clock source to PLL
	HAL_Delay(10);

	//SSD1963_Send_Cmd(SSD1963_RESET); 		//6. Software Reset
	//HAL_Delay(20);

	/*************
	 Dot clock Freq = PLL Freq x (LCDC_FPR + 1) / 2^20
	 PixClk	20MHz	174762	0x2AAAA
	 PixClk	25MHz	218452	0x35554
	 PixClk	30MHz	262143	0x3FFFF
	 PixClk	35MHz	305834	0x4AAAA
	 ********************/
	SSD1963_Send_Cmd(SSD1963_PIXEL_FREQ);	// 7. Configure the dot clock frequency
	SSD1963_Send_Dat(0x04);					// 0x03851D, 230685
	SSD1963_Send_Dat(0x0077);
	SSD1963_Send_Dat(0x0077);
	//8. Configure the LCD panel

	//a. Set the panel size to 480 x 800 and polarity of LSHIFT, LLINE and LFRAME to active low
	SSD1963_Send_Cmd(SSD1963_LCD_MODE_RES);
	SSD1963_Send_Dat(0x0020); 		//24 bit panel, disable dithering, LSHIFT: Data latch in rising edge, LLINE and LFRAME: active low
	SSD1963_Send_Dat(0x0000); 		// TFT type
	SSD1963_Send_Dat(799 >> 8);     // Horizontal Width:  800 - 1 = 799
	SSD1963_Send_Dat(799);
	SSD1963_Send_Dat(479 >> 8);     // Vertical Width :  480 -1 = 479
	SSD1963_Send_Dat(479);
	SSD1963_Send_Dat(0x00); 		//

	//b. Set the horizontal period
//	write_command(0x00B4);	//HSYNC
//	Write_Data_int(0x04);  //Set HT
//	Write_Data_int(0x1f);
//	Write_Data_int(0x00);  //Set HPS
//	Write_Data_int(0xd2);
//	Write_Data_int(0x00);			   //Set HPW
//	Write_Data_int(0x00);  //Set HPS
//	Write_Data_int(0x00);
//	Write_Data_int(0x00);

	SSD1963_Send_Cmd(SSD1963_HOR_TIMING);			// Horizontal Timing
	SSD1963_Send_Dat(0x04); 						// HT: horizontal total period (display + non-display) - 1 =  1055 (0x41f)
	SSD1963_Send_Dat(0x1f);
	SSD1963_Send_Dat(0x00);						 	// HPS: Horizontal Sync Pulse Start Position = Horizontal Pulse Width + Horizontal Back Porch = 16 = 0x10
	SSD1963_Send_Dat(0xd2);
	SSD1963_Send_Dat(0x00);     					// HPW: Horizontal Sync Pulse Width - 1=8-1=7
	SSD1963_Send_Dat(0x00); 						// LPS: Horizontal Display Period Start Position = 0x0000
	SSD1963_Send_Dat(0x00);
	SSD1963_Send_Dat(0x00); 						// LPSPP: Horizontal Sync Pulse Subpixel Start Position(for serial TFT interface).  Dummy value for TFT interface.

	//c. Set the vertical period
//	write_command(0x00B6);	//VSYNC
//	Write_Data_int(0x02);   //Set VT
//	Write_Data_int(0x0c);
//	Write_Data_int(0x00);  //Set VPS
//	Write_Data_int(0x22);
//	Write_Data_int(0x00);		//Set VPW
//	Write_Data_int(0x00);  //Set FPS
//	Write_Data_int(0x00);

	SSD1963_Send_Cmd(SSD1963_VRT_TIMING);			// Vertical Timing
	SSD1963_Send_Dat(0x02); 						// VT: Vertical Total (display + non-display) Period – 1  =647=0x287
	SSD1963_Send_Dat(0x0C);
	SSD1963_Send_Dat(0x00); 						// VPS: Vertical Sync Pulse Start Position
	SSD1963_Send_Dat(0x22);
	SSD1963_Send_Dat(0x00);    						//VPW: Vertical Sync Pulse Width
	SSD1963_Send_Dat(0x00);     					//FPS: Vertical Display Period Start Position = 0
	SSD1963_Send_Dat(0x00);

//	write_command(0x00B8);
//	Write_Data_int(0x000f);    //GPIO is controlled by host GPIO[3:0]=output   GPIO[0]=1  LCD ON  GPIO[0]=0  LCD OFF
//	Write_Data_int(0x0001);    //GPIO0 normal

	SSD1963_Send_Cmd(SSD1963_SET_GPIO_CFG);			// set GPIO configuration
	SSD1963_Send_Dat(0x000f);    					//GPIO is controlled by host GPIO[3:0]=output   GPIO[0]=1  LCD ON  GPIO[0]=1  LCD OFF
	SSD1963_Send_Dat(0x0001);    					//GPIO0 normal


//	write_command(0x00BA);
//	Write_Data_int(0x0001);    //GPIO[0] out 1 --- LCD display on/off control PIN

	SSD1963_Send_Cmd(SSD1963_SET_GPIO_VAL);			// 0x00BA);
	SSD1963_Send_Dat(0x0001);    //GPIO[0] out 1 --- LCD display on/off control PIN

//	write_command(0x0036); //rotation
//	Write_Data_int(0x0008);//RGB=BGR

	SSD1963_Send_Cmd(SSD1963_PIXEL_FORMAT);
	SSD1963_Send_Dat(0x05);

	SSD1963_Send_Cmd(SSD1963_SET_ADDR_MODE);		// set address mode - controls how frambuffer is displayed
	SSD1963_Send_Dat(0x0008);						// reverse the color order from frambuffer to LCD, i.e. RGB=BGR

//	write_command(0x003A); //Set the current pixel format for RGB image data
//	Write_Data_int(0x0050);//16-bit/pixel

	SSD1963_Send_Cmd(SSD1963_PIXEL_FORMAT);		// Set the current pixel format for RGB image data -- reserved in SSD1963 spec
	SSD1963_Send_Dat(0x0050);					// 16-bit/pixel

//	write_command(0x00F0); //Pixel Data Interface Format
//	Write_Data_int(0x0003);//16-bit(565 format) data

	SSD1963_Send_Cmd(SSD1963_SET_PIXL_DATA_TYPE);	// Pixel Data Interface Format
	SSD1963_Send_Dat(0x0003);						// 16-bit(565 format) data

//	write_command(0x00BC);
//	Write_Data_int(0x0040);//contrast value
//	Write_Data_int(0x0080);//brightness value
//	Write_Data_int(0x0040);//saturation value
//	Write_Data_int(0x0001);//Post Processor Enable

	SSD1963_Send_Cmd(SSD1963_SET_POST_PROC);		// 0x00BC);
	SSD1963_Send_Dat(0x0040);						//contrast value
	SSD1963_Send_Dat(0x0080);						//brightness value
	SSD1963_Send_Dat(0x0040);						//saturation value
	SSD1963_Send_Dat(0x0001);						//Post Processor Enable

	HAL_Delay(5);

	SSD1963_Send_Cmd(SSD1963_DISPLAY_ON);			// 0x0029); //display on

	//9. Set the back light control PWM clock frequency
	//PWM signal frequency = PLL clock / (256 * (PWMF[7:0] + 1)) / 256
//	write_command(0x00BE); //set PWM for B/L
//	Write_Data_int(0x0006);
//	Write_Data_int(0x0080);
//	Write_Data_int(0x0009);
//	Write_Data_int(0x00ff);
//	Write_Data_int(0x0000);
//	Write_Data_int(0x0000);

	SSD1963_Send_Cmd(SSD1963_SET_PWM);			// PWM configuration
	SSD1963_Send_Dat(0x0006); 					// set PWM signal frequency to 170Hz when PLL frequency is 100MHz
	SSD1963_Send_Dat(0x00FF);     				// PWM duty cycle  (100%)
	SSD1963_Send_Dat(0x0009);     				// 0x09 = enable DBC, 0x01 = disable DBC  //on
	SSD1963_Send_Dat(0x00ff);
	SSD1963_Send_Dat(0x0000);
	SSD1963_Send_Dat(0x0000);

	SSD1963_Send_Cmd(SSD1963_SET_DBL_CFG); 		// set dynamic backlight config
	SSD1963_Send_Dat(0x000d);					//

	HAL_GPIO_WritePin(LCD_BL_ON_GPIO_Port, LCD_BL_ON_Pin, GPIO_PIN_SET);

	SSD1963_Send_Cmd(SSD1963_SET_TEAR_ON);		// tunr on the tear effect pulse
	SSD1963_Send_Dat(0x0000);					// HBlank and VBlank Tear pulse

	SSD1963_Send_Cmd(0x0026);
	SSD1963_Send_Dat(0);
	// SSD1963_Clear(i);
	// SSD1963_Test();


}

void SSD1963_Bright(uint8_t bright) {
	SSD1963_Send_Cmd(SSD1963_SET_PWM);  		// PWM configuration
	SSD1963_Send_Dat(0x0C); 					// set PWM signal frequency to 170Hz when PLL frequency is 100MHz
	SSD1963_Send_Dat(bright);   				// PWM duty cycle
	SSD1963_Send_Dat(0x01);
}

//
// interface routines for touchGFX
//
int touchgfxDisplayDriverTransmitActive()
{
	return isTransmittingData;
}
void touchgfxDisplayDriverTransmitBlock(const uint8_t *pixels, uint16_t x,	uint16_t y, uint16_t width, uint16_t height)
{
	uint8_t lo = 0, hi = 0;
	uint32_t nPixels = width * height;

	isTransmittingData = 1;	// not necessary with this implementation since DISP_TransmitData is blocking anyway

	SSD1963_SetDisplayWindow(x, y, x + width - 1, y + height - 1, 1);
	SSD1963_Send_Cmd(SSD1963_WR_MEM);
	while (nPixels)
	{
		lo = *pixels++;
		hi = *pixels++;
		SSD1963_Send_Dat((hi << 8) | lo);
		nPixels--;
	}

	isTransmittingData = 0;	// not necessary with this implementation since DISP_TransmitData is blocking anyway

	touchgfx::startNewTransfer();

	isTransmittingData = 0;
}
int touchgfxDisplayDriverShouldTransferBlock(uint16_t bottom)
{
	// check TE pin if it is high then transmit
	//if(HAL_GPIO_ReadPin(LCD_TearEffect_GPIO_Port, LCD_TearEffect_Pin) == GPIO_PIN_SET)
	return 1;

	//return 0;
}

void SSD1963_LCDInvert(uint8_t bInvert)
{

	if(bInvert)
	{
		SSD1963_Send_Cmd(SSD1963_SET_ADDR_MODE);		// set address mode - controls how frambuffer is displayed
		SSD1963_Send_Dat(0x0008);						// reverse the color order from frambuffer to LCD, i.e. RGB=BGR

	}
	else
	{
		SSD1963_Send_Cmd(SSD1963_SET_ADDR_MODE);		// set address mode - controls how frambuffer is displayed
		SSD1963_Send_Dat(0x000B);						// reverse the color order from frambuffer to LCD, i.e. RGB=BGR

	};
}
