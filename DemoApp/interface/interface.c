
#include "interface.h"

typedef struct
{
	uint32_t bUpdateSystemFlag;
} ImportanceParameter_info_t;
#define USER_WANT2UPDATE_PARA (0x5a5a5a5a)
uint8_t menu[] =
	"\r\n******************************************\r\n"
	"              Hello World                 \r\n"
	"=================S32K Demo================\r\n"
	"======Func：PWM IO CAN SPI LIN CHT========\r\n"
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
	printf("\r\nLPC BuileTime:%s %s", __DATE__, __TIME__);
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
	__set_CONTROL(0); 
	__set_MSP(*(volatile uint32_t *)Address);
	S32_SCB->VTOR = (Address & 0x3FFFFF80);
	//	_EI();           
	INT_SYS_DisableIRQ(LPUART0_RxTx_IRQn); 
	Jump_To_Application();
	Jump_To_Application(); 
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

void chipInit(void)
{

	memset(&chipInfo, 0, sizeof(chipInfo));
	chipInfo.bForbidCanWakeup = FALSE;
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

static void SSD1306LCD_Init(void)
{
	OLED_Init();
	FF_Init();
	OLED_Fill(0);
	Delayms(10);
}

void Interface_Init(void)
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
	Debug_Uart();

	PrintWellcomeMsg();

	FirstPowerOnparementInit();

	DMA_Init();
	PutAllDebugInfor();

	ADC_init();
	CanInit();

	SPI1_Init();
	SPI2_Init();
	I2CMasterInit(100000);

	SSD1306LCD_Init();
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

		OLE_Display_Char((uint8_t *)"Hello!!!", (uint8_t *)"FlyAudio", (uint8_t *)"Control_borad");
	}
}

void ADCProc(void)
{
	static uint32_t timer = 0;

	if (ReadUserTimer(&timer) > 500)
	{
		uint16_t ADC_0 = 0;
		uint16_t ADC_1 = 0;
		uint16_t ADC_2 = 0;
		uint16_t ADC_3 = 0;
		uint16_t ADC_4 = 0;

		ResetUserTimer(&timer);

		ADC_0 = GetAdcChannelValue(CHANNEL_ADC_0);
		ADC_1 = GetAdcChannelValue(CHANNEL_ADC_1);
		ADC_2 = GetAdcChannelValue(CHANNEL_ADC_2);
		ADC_3 = GetAdcChannelValue(CHANNEL_ADC_3);
		ADC_4 = GetAdcChannelValue(CHANNEL_ADC_4);

		printf("ADC : 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", ADC_0, ADC_1, ADC_2, ADC_3, ADC_4);
	}
}

void FlySystemProc(void)
{
	SysTick_100ms();
	//FTM_input_capture();
	CanProc();
	//LinProc();
	ADCProc();
	//CapturePro();
	//GPIOProc();
	//DataRouter(); //数据整理转发路由功能
	FeedWdt();
	DebugPro();
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
