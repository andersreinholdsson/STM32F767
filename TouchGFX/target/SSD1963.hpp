/*
 * SSD1963.hpp
 *
 *  Created on: Jan 14, 2021
 *      Author: archer
 */

#ifndef TARGET_SSD1963_HPP_
#define TARGET_SSD1963_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_ll_fmc.h"

#include <stdlib.h>
#include <string.h>

#define SSD1963_CMD *(uint16_t *)0x60000000
#define SSD1963_DAT *(uint16_t *)(0x60000000 | (1 << 15))

#define SSD1963_RST_SET   HAL_GPIO_WritePin(LCD_nRST_GPIO_Port, LCD_nRST_Pin, GPIO_PIN_SET);
#define SSD1963_RST_RESET HAL_GPIO_WritePin(LCD_nRST_GPIO_Port, LCD_nRST_Pin, GPIO_PIN_RESET);

#define MIRROR_V 0
#define MIRROR_H 0

#define LSHIFT 0

#define SSD1963_HEIGHT      480
#define SSD1963_WIDTH       800

#define BLACK   0x000000 /*   0,   0,   0 */
#define WHITE   0xFFFFFF /* 255, 255, 255 */
#define RED     0xFF0000 /* 255,   0,   0 */
#define BLUE    0x0000FF /*   0,   0, 255 */
#define BLUE_D  0x0000A0 /*   0,   0, 160 */
#define CYAN    0x00FFFF /*   0, 255, 255 */
#define CYAN_D  0x008080 /*   0, 128, 128 */
#define YELLOW  0xFFFF00 /* 255, 255,   0 */
#define MAGENTA 0xFF00FF /* 255,   0, 255 */
#define GREEN   0x00FF00 /*   0, 255,   0 */
#define GREEN_D 0x008000 /*   0, 128,   0 */
#define PURPLE  0x800080 /* 128,   0, 128 */
#define TEAL    0x008080 /*   0, 128, 128 */
#define NAVY    0x000080 /*   0,   0, 128 */
#define SILVER  0xC0C0C0 /* 192, 192, 192 */
#define GRAY    0x808080 /* 128, 128, 128 */
#define ORANGE  0xFFA500 /* 255, 165,   0 */
#define BROWN   0xA52A2A /* 165, 255,  42 */
#define MAROON  0x800000 /* 128,   0,   0 */
#define OLIVE   0x808000 /* 128, 128,   0 */
#define LIME    0xBFFF00 /* 191, 255,   0 */

/*TFT 800x480*/
#define TFT_FPS 60ULL
#define TFT_WIDTH				800ULL
#define TFT_HSYNC_BACK_PORCH	42ULL
#define TFT_HSYNC_FRONT_PORCH	8ULL
#define TFT_HSYNC_PULSE			2ULL

#define TFT_HEIGHT				480ULL
#define TFT_VSYNC_BACK_PORCH	11ULL
#define TFT_VSYNC_FRONT_PORCH	6ULL
#define TFT_VSYNC_PULSE			6ULL


#define	TFT_HSYNC_PERIOD	(TFT_HSYNC_PULSE + TFT_HSYNC_BACK_PORCH + TFT_WIDTH  + TFT_HSYNC_FRONT_PORCH)
#define	TFT_VSYNC_PERIOD	(TFT_VSYNC_PULSE + TFT_VSYNC_BACK_PORCH + TFT_HEIGHT + TFT_VSYNC_FRONT_PORCH)

#define TFT_PCLK			(TFT_HSYNC_PERIOD * TFT_VSYNC_PERIOD * TFT_FPS)

#define LCD_FPR				((TFT_PCLK * 1048576)/100000000)
#define mHIGH(x) 			(x >> 8)
#define mLOW(x) 			(x & 0xFF)

typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t width, height;    // Bitmap dimensions in pixels
	uint8_t xAdvance;         // Distance to advance cursor (x axis)
	int8_t xOffset, yOffset; // Dist from cursor position to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t first, last; // ASCII extents
	uint8_t yAdvance;    // Newline distance (y axis)
} GFXfont;

inline uint16_t RGB(uint8_t r, uint8_t g, uint8_t b);

void SSD1963_Init(void);
void SSD1963_Bright(uint8_t bright);


#define SSD1963_RESET			0x0001
#define SSD1963_SLEEP_OUT		0x0011
#define SSD1963_GAMMA			0x0026
#define SSD1963_DISPLAY_OFF		0x0028
#define SSD1963_DISPLAY_ON		0x0029
#define SSD1963_COLUMN_ADDR		0x002A
#define SSD1963_PAGE_ADDR		0x002B
#define SSD1963_WR_MEM			0x002C
#define SSD1963_RD_MEM			0x002E

#define SSD1963_SET_TEAR_ON		0x0035
#define SSD1963_MAC			    0x0036
#define SSD1963_PIXEL_FORMAT    0x003A
#define SSD1963_WDB			    0x0051
#define SSD1963_WCD				0x0053
#define SSD1963_LCD_MODE_RES    0x00B0
#define SSD1963_FRC				0x00B1
#define SSD1963_BPC				0x00B5
#define SSD1963_DFC				0x00B6
#define SSD1963_POWER1			0x00C0
#define SSD1963_POWER2			0x00C1
#define SSD1963_VCOM1			0x00C5
#define SSD1963_VCOM2			0x00C7
#define SSD1963_POWERA			0x00CB
#define SSD1963_POWERB			0x00CF
//#define SSD1963_PGAMMA			0x00E0
#define SSD1963_NGAMMA			0x00E1
#define SSD1963_SET_FREQ		0x00E2
#define SSD1963_DTCA			0x00E8
#define SSD1963_DTCB			0x00EA
#define SSD1963_POWER_SEQ		0x00ED
#define SSD1963_3GAMMA_EN		0x00F2
#define SSD1963_INTERFACE		0x00F6
#define SSD1963_PRC				0x00F7
#define SSD1963_VERTICAL_SCROLL 0x0033
// added commands for init
#define SSD1963_PLL_ON			0x00E0
#define SSD1963_PIXEL_FREQ		0x00E6
#define SSD1963_VRT_TIMING		0x00B6
#define SSD1963_HOR_TIMING		0x00B4
#define SSD1963_SET_PWM			0x00BE
#define SSD1963_SET_GPIO_CFG	0x00B8
#define SSD1963_SET_GPIO_VAL	0x00BA
#define SSD1963_SET_ADDR_MODE	0x0036
#define SSD1963_SET_PIXL_DATA_TYPE		0x00F0
#define SSD1963_SET_POST_PROC	0x00BC
#define SSD1963_SET_DBL_CFG		0x00D0

void SSD1963_SetDisplayWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t bWrite);
void SSD1963_SendFrameBufferBlockWithPosition(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void SSD1963_Init(void);
void SSD1963_Bright(uint8_t bright);

void SSD1963_ConfigureInterrupts(void);
void SSD1963_EnableInterrupts(void);
void SSD1963_DisableInterrupts(void);
void SSD1963_EnableLCDInterrupts(void);
void SSD1963_Clear(uint16_t i, uint8_t bCheck);

void SSD1963_DmaComplete(DMA_HandleTypeDef *pDMA);
void SSD1963_Send_Dat(uint16_t data);
void SSD1963_Send_Cmd(uint16_t cmd);
void SSD1963_LCDInvert(uint8_t bInvert);

#ifdef __cplusplus
}
#endif

extern "C" int touchgfxDisplayDriverTransmitActive();
extern "C" int touchgfxDisplayDriverShouldTransferBlock(uint16_t bottom);
extern "C" void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
extern "C" void touchgfxSignalVSync(void);

#endif /* TARGET_SSD1963_HPP_ */
