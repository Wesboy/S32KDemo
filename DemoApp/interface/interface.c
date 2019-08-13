
#include "interface.h"

typedef struct
{
	uint32_t bUpdateSystemFlag;
} ImportanceParameter_info_t;
#define USER_WANT2UPDATE_PARA (0x5a5a5a5a)
uint8_t menu[] =
	"\r\n******************************************\r\n"
	"              Hello World                 \r\n"
	"             Created By: WYH		       \r\n"
	"==============Flyaudio?????==============\r\n"
	"******************************************\r\n";

#define PARAMETER_ADDRESS ParameterSection1Ram1Entry
#define PARAMETER_ADDRESSMAXSIZE ParameterSection3Ram1Size
parameter_info_t *paramInfo = (parameter_info_t *)PARAMETER_ADDRESS;

#define IMPORTANCEPARAMETER_ADDRESS ParameterSection3Ram1Entry
#define IMPORTANCEPARAMETER_SIZE ParameterSection3Ram1Size //32B = 0x20
/*????????*/
ImportanceParameter_info_t *ImportancePramInfo = (ImportanceParameter_info_t *)IMPORTANCEPARAMETER_ADDRESS;

extern void CLOCK_SYS_GetDefaultConfiguration(clock_manager_user_config_t *config);

#define DIGIT(s, no) ((s)[no] - '0')

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

static CHIP_INFO_STRU chipInfo;

uint32_t iTimer100ms;

/*****************************************************************************
**????????:	 	PrintWellcomeMsg
**????????:	 	
**??????:
**???????:
******************************************************************************/
void PrintWellcomeMsg(void)
{
	printf("%s", menu);
	printf("Build in:");
	printf("%d-", year);
	printf("%d-", months);
	printf("%d ", day);
	printf("%d:", hours);
	printf("%d:", minutes);
	printf("%d", seconds);
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
	__set_CONTROL(0); //???????????????...
	__set_MSP(*(volatile uint32_t *)Address);
	S32_SCB->VTOR = (Address & 0x3FFFFF80);
	//	_EI();           //?????????????,??????????????
	INT_SYS_DisableIRQ(LPUART0_RxTx_IRQn); //?????????????????????????????????????Rx??????????
	Jump_To_Application();
	Jump_To_Application(); //?????????????????????????
}

/***************************************************************************************************************************
**????????:	 	UserResetSystem
**????????:
**??????:
**???????:
***************************************************************************************************************************/
void UserResetSystem(void)
{
	printf("\r\n****UserResetSystem***");
	_DI();
	NVIC_SystemReset();
}

void IntoDeepSleep(void)
{

	printf("\r\n IntoDeepSleep");
	AllIOConfigInputForSleep();
	PutAllDebugInfor();
	//return;
	//    //??powerdown????????,IRC???????,?????IRC???????????????????
	RTCInit(WDT_1HH);

	{
		pin_settings_config_t config;
		memset(&config, 0, sizeof(pin_settings_config_t));
		config.base = PORTC;
		config.gpioBase = PTC;
		config.mux = PORT_MUX_AS_GPIO;
		config.direction = GPIO_INPUT_DIRECTION;
		config.pullConfig = PORT_INTERNAL_PULL_DOWN_ENABLED;
		config.pinPortIdx = 15;
		config.clearIntFlag = true;
		config.intConfig = PORT_INT_FALLING_EDGE; //falling
		if (!chipInfo.bForbidCanWakeup)
		{
			config.pinPortIdx = 2;
			PINS_Init(&config); //LCAN_RX_PIN
		}
		INT_SYS_EnableIRQ(PORTC_IRQn);
	}

	ipcEventSet(EVT_SET_GLOBAL_CLR_SLEEP, 0, NULL, 0);
	SMC->PMCTRL |= 0x2;							//????VLPS????????
												/* Set the SLEEPDEEP bit to enable deep sleep mode (STOP)*/
	S32_SCB->SCR |= S32_SCB_SCR_SLEEPDEEP_MASK; //????????????

	/* Cpu is going into deep sleep state */
	STANDBY(); //???????
	NVIC_DisableIRQ(PORTC_IRQn);
	NVIC_DisableIRQ(RTC_IRQn);

	GoT0WhatStatus(0);
}

/**************************************************************************************************************************
**????????:		IsResetOrWakeUp
**????????:
**??????:     ??
**???????:     ??
**************************************************************************************************************************/
static void IsResetOrWakeUp(void)
{
	ePowerStatus IsWhat = RESETUP;

	chipInfo.ResetStatus = paramInfo->GPREG2;
	paramInfo->GPREG2 = 0;
	if ((paramInfo->GPREG4 == 0x01) && (chipInfo.ResetStatus != P_SLEEP)) //??????
	{
		IsWhat = RESETUP;
	}
	else if (chipInfo.ResetStatus == P_SLEEP) //????(?????????????????)
	{
		IsWhat = WAKEUP;
	}

	chipInfo.chipPowerStatus = IsWhat;
}
void chipInit(void)
{

	memset(&chipInfo, 0, sizeof(chipInfo));
	chipInfo.bForbidCanWakeup = FALSE;

	IsResetOrWakeUp(); ///////////////////////////////////////////////////////////////////
					   //
}

ePowerStatus GetPowerOnBootMode(void)
{
	return chipInfo.chipPowerStatus;
}

void FirstPowerOnparementInit(void)
{
	if ((paramInfo->FirstPowerOn1 != 0x12345678) && (paramInfo->FirstPowerOn2 != 0x87654321) && (paramInfo->FirstPowerOn3 != 0xFF550122))
	{
		memset(paramInfo, 0, sizeof(parameter_info_t));
		paramInfo->FirstPowerOn1 = 0x12345678;
		paramInfo->FirstPowerOn2 = 0x87654321;
		paramInfo->FirstPowerOn3 = 0xFF550122;
		paramInfo->WakeUpSrc = 0;
	}
	else
	{
		printf("\n\rNoFirstPowerOn\n\r");
	}
}

void Interface_DebugInit(void)
{
	clock_manager_user_config_t config;
	chipInit();
	//	WatchdogInit();
	CLOCK_SYS_GetDefaultConfiguration(&config);
	config.scgConfig.clockModeConfig.rccrConfig.src = SCG_SYSTEM_CLOCK_SRC_SYS_PLL; //使用PLL做总线时钟 8M*16/2/2=32M
	config.scgConfig.spllConfig.mult = 8;											//0+16倍频
	config.scgConfig.spllConfig.prediv = 1;											//0+1分频
	config.scgConfig.soscConfig.freq = 8000000;										//外部时钟晶振
	CLOCK_SYS_SetConfiguration(&config);											//时钟初始化，默认osc=8M

	SysTickInit();
	Debug();

	ipcEventInit();
	PrintWellcomeMsg();
	if (RESETUP == GetPowerOnBootMode())
	{
		printf("\n\r System Reset");
	}
	else if (WAKEUP == GetPowerOnBootMode())
	{
		printf("\n\r System Awake");
	}
	FirstPowerOnparementInit();

	DMA_Init();
	PutAllDebugInfor();

	ADC_init();
	//PWM1ConfigInit();
	//FTM_IC_init();
	CanInit();

	SPI1_Init();
	SPI2_Init();
	I2CMasterInit(100000);
}

void AllIOConfigInputForSleep(void)
{
	PINS_DRV_SetPinsDirection(PTA, 0);
	PINS_DRV_SetPinsDirection(PTB, 0);
	PINS_DRV_SetPinsDirection(PTC, 0);
	PINS_DRV_SetPinsDirection(PTD, 0);
	PINS_DRV_SetPinsDirection(PTE, 0);
}

void SysTick_100ms(void)
{

	if (ReadUserTimer(&iTimer100ms) > 1000)
	{
		ResetUserTimer(&iTimer100ms);

		printf("test->1s->\r\n");
		OLE_Display_Char((u8 *)"Hello!!!", (u8 *)"FlyAudio", (u8 *)"Control_borad");
	}
}

void FlySystemProc(void)
{
	SysTick_100ms();
	FTM_input_capture();
	//CanProc();
	FeedWdt();
	DebugPro();
}

static void ipcEventChipGet(event_t evt, uint32_t *param, uint8_t *p, uint16_t len)
{
	switch (evt)
	{
	case EVT_GET_CHIP_BOOTMODE:
		*param = (uint32_t)GetPowerOnBootMode();
		break;

	default:
		break;
	}
}
static void ChipPeriodDebug(void)
{
	printf("\r\n WakeSrc:%d", paramInfo->WakeUpSrc);
}

static void ipcEventChipSet(event_t evt, uint32_t param, uint8_t *p, uint16_t len)
{
	switch (evt)
	{
	case EVT_SET_GLOBAL_SYSTEM_INIT_FINISH:
		/* Android/WinCE Application Is Ready OK */
		break;

	case EVT_SET_GLOBAL_SYSTEM_READY_TO_RECEIVE_STATUS:
		/* Android/WinCE Hardware Is Ready OK */
		break;

	case EVT_SET_GLOBAL_CLR_ACC:
		chipInfo.AccStatus = (bool)param;
		break;

	case EVT_SET_CHIP_RESET:
		UserResetSystem();
		break;

	case EVT_SET_GLOBAL_PERIOD_DEBUG:
		ChipPeriodDebug();
		break;

	case EVT_SET_CHIP_FORBID_CANWAKEUP:
		if (param)
		{
			chipInfo.bForbidCanWakeup = TRUE;
		}
		else
		{
			chipInfo.bForbidCanWakeup = FALSE;
		}
		break;

	default:
		break;
	}
}
void chipIpcEventRegister(void)
{
	ipcEventRegister(EVT_MODE_CHIP, ipcEventChipGet, ipcEventChipSet);
}

void ModuleIpcEventRegister(void)
{
	CanIpcEventRegister();
	chipIpcEventRegister();
}

void FlySystem_DisableOutput(void)
{

}

void _DI()
{
	INT_SYS_DisableIRQGlobal();
}
void _EI()
{
	INT_SYS_EnableIRQGlobal();
}
static void FlySystemDeInit(void)
{
	ADC_Deinit();
	DMA_DeInit();
	PWM1ConfigDeInit();
	CanDeInit();

	FlySystem_DisableOutput();
}
void chipDeInit(void)
{
	printf("\r\nchipDeInit");

	paramInfo->GPREG2 = P_SLEEP;
	SysTickDeInit();
	FlySystemDeInit();
}

void Programming_SetUpdataSystemFlag(void)
{
	ImportancePramInfo->bUpdateSystemFlag = USER_WANT2UPDATE_PARA;
}
void Programming_ClearUpdataSystemFlag(void)
{
	ImportancePramInfo->bUpdateSystemFlag = 0;
}
