/**
  ******************************************************************************
  * File Name          : TouchGFXHAL.cpp
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
#include <TouchGFXHAL.hpp>

/* USER CODE BEGIN TouchGFXHAL.cpp */
#include <main.h>
#include <touchGFX/hal/OSWrappers.hpp>
#include "STM32TouchController.hpp"
#include "SSD1963.hpp"
#include "FT5XX6.hpp"

#include "stm32f7xx.h"
extern void LCDTearEffect(void);
extern void CapTouch(void);
extern void ResTouch(void);

using namespace touchgfx;

/* ******************************************************
 * Functions required by Partial Frame Buffer Strategy
 * ******************************************************
 *
 *  int touchgfxDisplayDriverTransmitActive() must return whether or not data is currently being transmitted, over e.g. SPI.
 *  void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h) will be called
 *  when the framework wants to send a block. The user must then transfer the data represented by the arguments.
 *
 *  A user must call touchgfx::startNewTransfer(); once touchgfxDisplayDriverTransmitBlock() has successfully sent a block.
 *  E.g. if using DMA to transfer the block, this could be called in the "Transfer Completed" interrupt handler.
 *
 */
// #error "A user must implement C-methods touchgfxDisplayDriverTransmitActive() and touchgfxDisplayDriverTransmitBlock() used by the Partial Framebuffer Strategy."
void TouchGFXHAL::initialize() {
	// Calling parent implementation of initialize().
	//
	// To overwrite the generated implementation, omit call to parent function
	// and implemented needed functionality here.
	// Please note, HAL::initialize() must be called to initialize the framework.

	// iinitialize the LCD
	SSD1963_Init();
    SSD1963_Clear(0x1F<<11, 0);		// RED
    SSD1963_Clear(0x2F<<5, 0);		// green
    SSD1963_Clear(0x1F, 0);		// blue
    // SSD1963_Clear(0xffff, 0);		// white
    // SSD1963_Clear(0, 0);		// black


	// setup the DMA channel
	//hdma_memtomem_dma2_stream0.Lock = HAL_UNLOCKED;
	//hdma_memtomem_dma2_stream0.XferCpltCallback = SSD1963_DmaComplete;
	// create the mutext -- already done in main

	TouchGFXGeneratedHAL::initialize();
}

void TouchGFXHAL::taskEntry()
{
    enableLCDControllerInterrupt();
    enableInterrupts();

    OSWrappers::waitForVSync();
    backPorchExited();

    /* Enable the LCD, Send Display on DCS command to display */
//    HAL_DSI_ShortWrite(&hdsi, LCD_OTM8009A_ID, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_DISPON, 0x00);

    for (;;)
    {
        OSWrappers::waitForVSync();
        backPorchExited();
    }
}

/**
 * Gets the frame buffer address used by the TFT controller.
 *
 * @return The address of the frame buffer currently being displayed on the TFT.
 */
uint16_t* TouchGFXHAL::getTFTFrameBuffer() const
{
    // Calling parent implementation of getTFTFrameBuffer().
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.

    return TouchGFXGeneratedHAL::getTFTFrameBuffer();
}

/**
 * Sets the frame buffer address used by the TFT controller.
 *
 * @param [in] address New frame buffer address.
 */
void TouchGFXHAL::setTFTFrameBuffer(uint16_t* address)
{
    // Calling parent implementation of setTFTFrameBuffer(uint16_t* address).
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.

    TouchGFXGeneratedHAL::setTFTFrameBuffer(address);
}

/**
 * This function is called whenever the framework has performed a partial draw.
 *
 * @param rect The area of the screen that has been drawn, expressed in absolute coordinates.
 *
 * @see flushFrameBuffer().
 */
void TouchGFXHAL::flushFrameBuffer(const touchgfx::Rect& rect)
{
    // Calling parent implementation of flushFrameBuffer(const touchgfx::Rect& rect).
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.
    // Please note, HAL::flushFrameBuffer(const touchgfx::Rect& rect) must
    // be called to notify the touchgfx framework that flush has been performed.

    TouchGFXGeneratedHAL::flushFrameBuffer(rect);
}

bool TouchGFXHAL::blockCopy(void* RESTRICT dest, const void* RESTRICT src, uint32_t numBytes)
{
    return TouchGFXGeneratedHAL::blockCopy(dest, src, numBytes);
}

/**
 * Configures the interrupts relevant for TouchGFX. This primarily entails setting
 * the interrupt priorities for the DMA and LCD interrupts.
 */
 // IRQ callback for LCDTearEffect (i.e. safe to draw
void LCDTearEffect(void)
{
	/* Unblock TouchGFX Engine Main Loop to render next frame */
	OSWrappers::signalVSync();
}
void CapTouch(void)
{

}
void ResTouch(void)
{

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)		// LCD_Tear
	{
		OSWrappers::signalVSync();
	}
	else if(GPIO_Pin == GPIO_PIN_1)
	{
		TOUCH_Pressed();
		// capacitive touch
	}
	else if(GPIO_Pin == GPIO_PIN_3)
	{
		// resistance touch
	}
}

void TouchGFXHAL::configureInterrupts()
{
    // Calling parent implementation of configureInterrupts().
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.

	// setup call backs on interrupts
	//EXT0 -- LCD
	HAL_EXTI_GetHandle(&hExti_LCD, LCD_Exti);
	HAL_EXTI_GetHandle(&hExti_CTP, CTP_Exti);
	HAL_EXTI_GetHandle(&hExti_RTP, RTP_Exti);

	HAL_EXTI_RegisterCallback(&hExti_LCD, HAL_EXTI_COMMON_CB_ID, LCDTearEffect);
	HAL_EXTI_RegisterCallback(&hExti_CTP, HAL_EXTI_COMMON_CB_ID, CapTouch);
	HAL_EXTI_RegisterCallback(&hExti_RTP, HAL_EXTI_COMMON_CB_ID, ResTouch);

	//  TouchGFXGeneratedHAL::configureInterrupts();
}

/**
 * Used for enabling interrupts set in configureInterrupts()
 */
void TouchGFXHAL::enableInterrupts()
{
    // Calling parent implementation of enableInterrupts().
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.

	//enable touch interrupt

	// enable LCD_TearEffect IRQ, and touch screens
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

//    TouchGFXGeneratedHAL::enableInterrupts();
}

/**
 * Used for disabling interrupts set in configureInterrupts()
 */
void TouchGFXHAL::disableInterrupts()
{
    // Calling parent implementation of disableInterrupts().
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.

	// enable LCD_TearEffect IRQ, and touch screens
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	// TouchGFXGeneratedHAL::disableInterrupts();
}

/**
 * Configure the LCD controller to fire interrupts at VSYNC. Called automatically
 * once TouchGFX initialization has completed.
 */
void TouchGFXHAL::enableLCDControllerInterrupt()
{
    // Calling parent implementation of enableLCDControllerInterrupt().
    //
    // To overwrite the generated implementation, omit call to parent function
    // and implemented needed functionality here.

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
//    TouchGFXGeneratedHAL::enableLCDControllerInterrupt();
}
void startNewTransfer()
{
    FrameBufferAllocator* fba = HAL::getInstance()->getFrameBufferAllocator();
    fba->freeBlockAfterTransfer();
    // blockIsTransferred = true;

    if (fba->hasBlockReadyForTransfer())
    {
        touchgfx::Rect r;
        const uint8_t* pixels = fba->getBlockForTransfer(r);
        touchgfxDisplayDriverTransmitBlock((uint8_t*)pixels, r.x, r.y, r.width, r.height);
    //    fba->freeBlockAfterTransfer();
    }
}
void TouchGFXHAL::flipScreen(uint8_t bInvert)
{
	SSD1963_LCDInvert(bInvert);
}
/* USER CODE END TouchGFXHAL.cpp */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
