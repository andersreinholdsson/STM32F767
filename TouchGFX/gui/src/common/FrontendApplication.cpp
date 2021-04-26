#include <new>
#include <gui_generated/common/FrontendApplicationBase.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Texts.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>
#include <gui/splash_screen/SplashView.hpp>
#include <gui/splash_screen/SplashPresenter.hpp>
#include <main.h>


FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{

}

void FrontendApplication::handleTickEvent()
{
    model.tick();
    T_MSG gfxMsg;

	osStatus_t status = osMessageQueueGet(GfxQHandle, &gfxMsg, 0U, 50U);   // wait for message 50 mS

	if(status == osOK)
	{
		switch(gfxMsg.msgType)
		{
			default:
				break;
		}
	}

    FrontendApplicationBase::handleTickEvent();
}

