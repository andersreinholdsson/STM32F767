#include <stdint.h>
#include "stm32f4xx_hal.h"

#define SYSMEM_RESET_VECTOR            0x1fff0004 //  0x1fffC804 -- orig	//STM32 App Note 2606 0x1ff00000
#define RESET_TO_BOOTLOADER_MAGIC_CODE 0xDEADBEEF
#define BOOTLOADER_STACK_POINTER       0x20002250

uint32_t static volatile dfu_reset_to_bootloader_magic;

void DfuCheck(void)
{
	HAL_PWR_EnableBkUpAccess();
	RTC->TAFCR = 0;
    if ((dfu_reset_to_bootloader_magic == RESET_TO_BOOTLOADER_MAGIC_CODE) || (RTC->BKP19R == RESET_TO_BOOTLOADER_MAGIC_CODE))
    {
        void (*bootloader)(void) = (void (*)(void)) (*((uint32_t *) SYSMEM_RESET_VECTOR));
        dfu_reset_to_bootloader_magic = 0;
        RTC->BKP19R = 0;
        __set_MSP(BOOTLOADER_STACK_POINTER);
		bootloader();
        while (42);
    }
    dfu_reset_to_bootloader_magic = 0;
}

void DfuRunBootLoader()
{
	HAL_PWR_EnableBkUpAccess();
	RTC->TAFCR = 0;
	dfu_reset_to_bootloader_magic = RESET_TO_BOOTLOADER_MAGIC_CODE;
    RTC->BKP19R = RESET_TO_BOOTLOADER_MAGIC_CODE;
    HAL_PWR_DisableBkUpAccess();
    NVIC_SystemReset();
}
