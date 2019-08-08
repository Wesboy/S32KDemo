#ifndef _MCU_STATUS_MANAGE_H_
#define _MCU_STATUS_MANAGE_H_

//#define ACCSLEEPTIME  (T_1S * 30)     //ACC=0时维持多长时间后进入休眠
#define ACCSLEEPTIME  (T_1S * 5)     //ACC=0时维持多长时间后进入休眠
#define ACCFILTERTIME (T_1S * 2)      //避免频繁开关机的时间阈值
#define MSM1PROTECTTTIME (T_1S * 3)   //在MSM1没有拉高超时,表示高通开机异常
#define ASYNCRESETMSM1PROTECTTTIME (T_1S * 15)   //在LPC进行异步复位时延长MSM1引脚的检测时间

typedef enum
{
	MCUMANAGE_ST_S0 = 1,
	MCUMANAGE_ST_S1,
	MCUMANAGE_ST_S2_1_0,
	MCUMANAGE_ST_S2_1_0_0,
	MCUMANAGE_ST_S2_1_0_1,
	MCUMANAGE_ST_S2_1_0_2,
	MCUMANAGE_ST_S2_1_1,
	MCUMANAGE_ST_S2_1_2,
	MCUMANAGE_ST_S2_2_0,
	MCUMANAGE_ST_S2_2_1,
	MCUMANAGE_ST_S2_3,
	MCUMANAGE_ST_S3_1,
	MCUMANAGE_ST_S3_2,
	MCUMANAGE_ST_S4_0, //异常状态
}enumMcuStatus;


void McuStatusManageInit(void);
void McuStatusManageProc(void);
void McuStatusManageIpcEventRegister(void);


#endif

