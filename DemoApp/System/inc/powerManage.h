#ifndef _POWER_MANAGE_H_
#define _POWER_MANAGE_H_

#include "common.h"

#define ADC_ACC_R_UP		330 //检测电瓶电压电路中利用分压电压进行测量电压,上拉电阻为330K
#define ADC_ACC_R_DOWN		33  //下拉电阻为33K


#define		POWER_ST_NORMAL				(T_1S * 0)
#define		POWER_ST_LOW				(T_1S * 10)
#define		POWER_ST_OFF_NOW			(T_1MS * 10)
#define		POWER_ST_OFF_NOW_FOR_ERROR	(T_1MS * 9)
#define		POWER_ST_OFF				(T_1S * 300)
#define		POWER_ST_SLEEP				(T_1S * 20)
#define		POWER_ST_RESET				(T_1S * 30)
#define		POWER_ST_DELAY_RESET		(T_1S * 35)
#define		POWER_ST_FASTSLEEP			(T_1S * 5)

#define		HAVE_RESET		0x12345678
#define     HAVE_POWERDOWN  0x11223344

enum enumScreenRestPowerOnStep
{
		eScreenRestEnd=0,
    eScreenRestInit,
		eScreenRestPinLow,
		eScreenRestPinLowDelayms,
		eScreenRestPinHight,
		eScreenRestPinHightDelayms,
		ePanelCTL,
		eScreenRestPinFish,
		
};

enum enumQualPowerOnStep
{
    ePowerInit,
    ePowerOnResetOn,
    ePowerOnResetOnDelay,
    ePowerOnResetOff,
    ePowerOnSignalOff,
    ePowerOnSignalDelay,
    ePowerOnSignalOn,
    ePowerOnWakeUp,
    ePowerOnFinish
};

typedef enum
{
	RESET_AMP_INIT = 1,
	RESET_AMP_MUTE,
	RESET_AMP_MUTE_DELAY,
	RESET_AMP_STANDBY,
	RESET_AMP_STANDBY_DELAY,
	RESET_AMP_FINSH
}eRESET_AMP;

typedef enum
{
	POWER_AMP_INIT = 0,
	POWER_AMP_MUTE_ON,
	POWER_AMP_MUTE_ON_DELAY,
	POWER_AMP_MUTE_OFF,
	POWER_AMP_MUTE_OFF_DELAY,
	POWER_AMP_STANDBY_ON,
	POWER_AMP_STANDBY_ON_DELAY,
	POWER_AMP_STANDBY_OFF,
	POWER_AMP_FINSH
}ePOWER_AMP;

typedef enum {
	PWRResetMode_None = 0,
	PWRResetMode_Now = 1,
	PWRResetMode_Delay = 2,
	PWRResetMode_Over = 3,
}ePowerResetMode;

typedef enum {
	PowerResetWait_Init = 0,
	PowerResetWait_Start = 1,
	PowerResetWait_Waiting = 2,
	PowerResetWait_Finish = 3
}ePowerResetWait;

#endif

void PowerManageProc(void);
void powerManageInit(void);
void powerManageIpcEventRegister(void);

