
#include "interface.h"


/*****************************************************************************
**函数名称:	 	RTCInit
**函数功能:	 	
**入口参数:
**返回参数:
**说明 中断时间为 Count*1000ms    
******************************************************************************/
void RTCInit(U16 Count)
{
		rtc_init_config_t rtc_init_config;
		{
					module_clk_config_t module_clk_config;
					module_clk_config.clkSrcFreq =LOWEST_FREQUENCY;//
					module_clk_config.clkSrcType=XOSC_CLK_SRC_TYPE;
					CLOCK_DRV_SetModuleClock(RTC0_CLK,&module_clk_config);
		}
					rtc_init_config.updateEnable=true;
					rtc_init_config.clockOutConfig=RTC_CLKOUT_DISABLED;//输出时钟禁止
					rtc_init_config.clockSelect=RTC_CLK_SRC_LPO_1KHZ;//使用1khz
					RTC_DRV_Init(0,&rtc_init_config);
					RTC->IER=0X00;											//
					INT_SYS_ClearPending(RTC_IRQn);			//清除挂起中断
						INT_SYS_DisableIRQ(RTC_IRQn);
            /* Write alarm time into Time Alarm Register */
            RTC->TAR=Count*93/100;											//计数目标 93%误差
            /* Enable/disable interrupt source based on the configuration */
            RTC->IER=0X04;										//中断使能
            /* After the configuration is done, re-enable the interrupt in
             * NVIC.
             */
            INT_SYS_EnableIRQ(RTC_IRQn);			//开启中断
						RTC->TSR=0;												//计数开始值
						RTC->SR=0x10;											//开始计数

}


void RTC_IRQHandler()
{

	RTC->TAR=0;
	RTC->SR=0x00;   
	paramInfo->WakeUpSrc = 0x03;

}


