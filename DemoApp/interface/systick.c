

#include "interface.h"

static uint32_t _sys_tick_count = 0;
/*****************************************************************************
**函数名称:	 	ReadUserTimer
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
uint32_t ReadUserTimer(uint32_t *Timer)
{
	return (_sys_tick_count - *Timer);
}
/*****************************************************************************
**函数名称:	 	ResetUserTimer
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void ResetUserTimer(uint32_t *Timer)
{
	*Timer = _sys_tick_count;
}
/*****************************************************************************
**函数名称:	 	SetUserTimer
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void SetUserTimer(uint32_t *Timer, uint32_t T)
{
	*Timer = _sys_tick_count + T;
}
/*****************************************************************************
**函数名称:	 	GetTickCount
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
uint32_t GetTickCount(void)
{
	return _sys_tick_count;
}
uint32_t TimerGetSystemTimer(void)
{
	return _sys_tick_count;
}
/*****************************************************************************
**函数名称:	 	Delayms
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void Delayms(uint32_t Time)
{
	uint32_t t;
	ResetUserTimer(&t);
	while (ReadUserTimer(&t) < Time)
	{
		FeedWdt();
	}
}

/*****************************************************************************
**函数名称:	 	SysTick_Handler
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void SysTick_Handler()
{
	//	static u8 iCnt;
	//	SYSTICK_ClearCounterFlag();
	do
	{
		_sys_tick_count++;
	} while (0 == _sys_tick_count);
	//CheckADCACCIrq();
}
/*****************************************************************************
**函数名称:	 	SYSTICK_InternalInit
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/

void SysTickInit(void)
{
	//1ms
	uint32_t core_freq = 0u;
	/* Get the correct name of the core clock */
	clock_names_t coreclk = CORE_CLK;
	status_t clk_status = CLOCK_SYS_GetFreq(coreclk, &core_freq);
	//S32_SysTick->RVR = S32_SysTick_RVR_RELOAD(core_freq / 1000-1000);
	S32_SysTick->RVR = S32_SysTick_RVR_RELOAD(core_freq / 1000);
	S32_SysTick->CSR = S32_SysTick_CSR_ENABLE(1u) | S32_SysTick_CSR_TICKINT(1u);
	S32_SysTick->CSR &= ~S32_SysTick_CSR_CLKSOURCE_MASK;
}
/*****************************************************************************
**函数名称:	 	SysTickDeInit
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void SysTickDeInit(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*****************************************************************************
*  Name        : TimerHasExpired
*  Description : Checks whether given timer has expired
*        With timer tick at 1ms maximum timer period is 10000000 ticks
*        When *STimer is set (SoftTimer-*STimer) has a min value of 0xFFF0BDBF
*            and will be more than this while the timer hasn't expired
*        When the timer expires
*                (SoftTimer-*STimer)==0
*            and (SoftTimer-*STimer)<=7FFF FFFF for the next 60 hours
*  Params      : STimer pointer to timer value
*  Returns     :  TRUE if timer has expired or timer is stopped, otherwise FALSE
*****************************************************************************/
uint8_t TimerHasExpired(uint32_t *STimer)
{
	if (*STimer == 0)
		return TRUE;
	else if ((_sys_tick_count - *STimer) <= 0x7fffffff)
	{
		*STimer = 0; //set timer to stop
		return TRUE;
	}
	else
		return FALSE;
}

/*****************************************************************************
*  Name        : TimerSet
*  Description : Sets a timer. timer must not equ 0 because 0 means timer is stop
*  Params      : STimer pointer to timer value
*                TimeLength - timer period
*  Returns     : none
*****************************************************************************/
void TimerSet(uint32_t *STimer, uint32_t TimeLength)
{
	*STimer = _sys_tick_count + TimeLength;

	if (*STimer == 0)
		*STimer = 1; //not set timer to 0 for timer is running
}
/*****************************************************************************
*  Name        : TimerStop
*  Description : stop timer
*  Params      : STimer pointer to timer value
*  Returns     : 
*****************************************************************************/
void TimerStop(uint32_t *STimer)
{
	*STimer = 0;
}
