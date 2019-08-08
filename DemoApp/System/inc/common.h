#ifndef __COMMON_H__
#define __COMMON_H__

#include "lpc_types.h"
//#include "lpc17xx_can.h"
//#include "lpc17xx_pinsel.h"
//#include "lpc17xx_clkpwr.h"

#include "fly_types.h"
#include "ipcEventDef.h"
#include "ipcEvent.h"
#include "interface.h"

#define 	T_1MS		(1)
#define 	T_10MS		(10)
#define 	T_100MS		(100)
#define 	T_1S		(1000)
#define 	T_1MM		(T_1S*60)

#define P_SLEEP		0x12345678


void FeedWdt(void);


void TimerSet(uint32_t *STimer, uint32_t TimeLength);
void TimerStop(uint32_t *STimer);
uint8_t TimerHasStopped(uint32_t STimer);
uint8_t TimerHasExpired (uint32_t *STimer);
void TimerWaitExpired (uint32_t *STimer);
void TimerDelayMs (uint32_t ms);
uint32_t TimerGetSystemTimer(void);


#endif

