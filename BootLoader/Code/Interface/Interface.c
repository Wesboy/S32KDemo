#include "Interface.h"

#define DIGIT(s, no) ((s)[no] - '0')

ImportanceParameter_info_t *ImportancePramInfo = (ImportanceParameter_info_t *)IMPORTANCEPARAMETER_ADDRESS;
#define SysGuideCfgAddressEntry ParameterSection2FlashEntry

const unsigned CRP_VERSION1 __attribute__((at(SysGuideCfgAddressEntry))) = 0x77777777;
const unsigned CRP_VERSION2 __attribute__((at(SysGuideCfgAddressEntry + 0X04))) = 0x33333333;

static void Chip_Init(void);
static void PrintWellcomeMsg(void);

void BCM_BootloadInit(void)
{
	Chip_Init();
	DebugInit();
	PrintWellcomeMsg();
}
void BCM_BootloadDeInit(void)
{

	FLEXCAN_DRV_Deinit(0);
	FLEXCAN_DRV_Deinit(1);
	FLEXCAN_DRV_Deinit(2);
	DMA_DeInit();
	WDOG_DRV_Deinit(0);
	//PrintWellcomeMsg();
}

void BCM_SystemPro(void)
{
	CanProc();
	ProgramPro();
	FlyPDUNetwork_Pro(); //诊断通讯流程
}

static void Chip_Init(void)
{
	clock_manager_user_config_t config;
	CLOCK_SYS_GetDefaultConfiguration(&config);
	config.scgConfig.clockModeConfig.rccrConfig.src = SCG_SYSTEM_CLOCK_SRC_SYS_PLL; //使用PLL做总线时钟 8M*16/2/2=32M
	config.scgConfig.spllConfig.mult = 10;											//0+16倍频
	config.scgConfig.spllConfig.prediv = 1;											//1+1分频
	config.scgConfig.soscConfig.freq = 8000000;										//外部时钟晶振
	CLOCK_SYS_SetConfiguration(&config);											//时钟初始化，默认osc=8M
}

static void PrintWellcomeMsg(void)
{

	uint8_t menu[] =
		"\r\n******************************************\r\n"
		"             BCM-BootLoader-S32K              \r\n"
		"             Created By:Flyaudio   		   \r\n"
		"**********************************************\r\n";

	int hours = (10 * DIGIT(__TIME__, 0) + DIGIT(__TIME__, 1));
	int minutes = (10 * DIGIT(__TIME__, 3) + DIGIT(__TIME__, 4));
	int seconds = (10 * DIGIT(__TIME__, 6) + DIGIT(__TIME__, 7));
	/* WARNING: This will fail in year 10000 and beyond, as it assumes
	* that a year has four digits. */
	int year = (1000 * DIGIT(__DATE__, 7) + 100 * DIGIT(__DATE__, 8) + 10 * DIGIT(__DATE__, 9) + DIGIT(__DATE__, 10));
	/*
	* Jan - 1
	* Feb - 2
	* Mar - 3
	* Apr - 4
	* May - 5
	* Jun - 6
	* Jul - 7
	* Aug - 8
	* Sep - 9
	* Oct - 10
	* Nov - 11
	* Dec - 12
	*/

	/* Use the last letter as primary "key" and middle letter whenever
* two months end in the same letter. */
	const int months = (__DATE__[2] == 'b' ? 2 : (__DATE__[2] == 'y' ? 5 : (__DATE__[2] == 'l' ? 7 : (__DATE__[2] == 'g' ? 8 : (__DATE__[2] == 'p' ? 9 : (__DATE__[2] == 't' ? 10 : (__DATE__[2] == 'v' ? 11 : (__DATE__[2] == 'c' ? 12 : (__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 1 : 6) :
																																																															  /* Implicit "r" */
																																																											   (__DATE__[1] == 'a' ? 3 : 4))))))))));
	const int day = (10 * (__DATE__[4] == ' ' ? 0 : DIGIT(__DATE__, 4)) + DIGIT(__DATE__, 5));

	printf("%s", menu);
	printf("              Build in:");
	printf("%d-", year);
	printf("%d-", months);
	printf("%d ", day);
	printf("%d:", hours);
	printf("%d:", minutes);
	printf("%d", seconds);
}

void _DI()
{
	INT_SYS_DisableIRQGlobal();
}
void _EI()
{
	INT_SYS_EnableIRQGlobal();
}

void CAN_Init(void)
{
	CAN0_Init(500000);
}
void GoT0WhatStatus(uint32_t Address)
{
	typedef void (*pFunction)(void);

	pFunction Jump_To_Application;
	uint32_t JumpAddress;
	_DI();
	JumpAddress = *(volatile uint32_t *)(Address + 4);
	Jump_To_Application = (pFunction)JumpAddress;
	__set_PSP(*(volatile uint32_t *)Address);
	__set_CONTROL(0); //设置使用主堆栈指针...
	__set_MSP(*(volatile uint32_t *)(Address));
	S32_SCB->VTOR = (Address & 0x3FFFFF80);
	//	_EI();           //不在此处进行使能,而是跳转后再使能
	INT_SYS_DisableIRQ(LPUART0_RxTx_IRQn); //串口升级模式中，跳转到其他地址会有概率触发Rx导致程序跑飞
	BCM_BootloadDeInit();
	Jump_To_Application();
	Jump_To_Application(); //防止编译器高等级优化导致栈错误。
}
void Diagnose_Communication_CAN_Send(unsigned char *buff, unsigned char dlc)
{
	CanTxMsg msg;
	CanMsgStructInit(&msg, buff, 8, Diag_BCM_Request_ID, CAN_ID_STD);
	CAN0AddMsg(msg);
}
void Programming_SetUpdataSystemFlag(void)
{
	ImportancePramInfo->bUpdateSystemFlag = USER_WANT2UPDATE_PARA;
}
void Programming_ClearUpdataSystemFlag(void)
{
	ImportancePramInfo->bUpdateSystemFlag = 0;
}
u32 Bootloader_GetUpdataSystemFlag()
{
	return ImportancePramInfo->bUpdateSystemFlag;
}

void Programming_ClearHaveSystemFlag(void)
{

	//	CRP_VERSION1=0;
	//	CRP_VERSION2=0;
}
u32 Bootloader_GetHaveSystemFlag()
{
	return ImportancePramInfo->bHaveSystemFlag;
}
