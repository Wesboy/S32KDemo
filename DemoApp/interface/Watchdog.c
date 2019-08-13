#include "interface.h"
void WatchdogInit()
{
	wdog_user_config_t wdog_user_config;
	wdog_user_config.clkSource = WDOG_SOSC_CLOCK;
	wdog_user_config.intEnable = false;
	wdog_user_config.opMode.wait = false;
	wdog_user_config.opMode.stop = false;
	wdog_user_config.opMode.debug = true;
	wdog_user_config.prescalerEnable = true;
	wdog_user_config.timeoutValue = 0x7a12; //8M CLOCK 看门狗约1s复位
	wdog_user_config.updateEnable = true;
	wdog_user_config.windowValue = 0;
	wdog_user_config.winEnable = false;

	WDOG_DRV_Init(0, &wdog_user_config);
}
/*****************************************************************************
**函数名称:	 	FeedWdt
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void FeedWdt()
{
	WDOG_DRV_Trigger(0);
}
