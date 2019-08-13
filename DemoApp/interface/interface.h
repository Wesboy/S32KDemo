#ifndef __INTERFACE__H__
#define __INTERFACE__H__
#include "fly_types.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "ipcEvent.h"
#include "ConfigFile.h"
#include "misc.h"
#include "fifo.h"
#include "canInternal.h"
#include "FTM_InputCapture.h"
//#include "codetab.h"

#include "clock_manager.h"
#include "MemoryConfig.h"

#include "s32k144_lpuart.h"
#include "interrupt_manager.h"
#include "osif.h"
#include "wdog_driver.h"
#include "adc_driver.h"
#include "adc_hw_access.h"
#include "pins_port_hw_access.h"
#include "pins_gpio_hw_access.h"
#include "lpi2c_driver.h"
#include "lpi2c_hw_access.h"
#include "ftm_pwm_driver.h"
#include "ftm_hw_access.h"
#include "ftm_common.h"
#include "rtc_driver.h"
#include "flexcan_driver.h"
#include "flexcan_hw_access.h"
#include "lpspi_master_driver.h"
#include "lpspi_shared_function.h"
#include "lpspi_slave_driver.h"
#include "lptmr_driver.h"
#include "pdb_driver.h"

#include "SSD1306_OLED.h"
#include "SSD1306_flash.h"

#include "DMA.h"
#include "SPI.h"
#include "Watchdog.h"
#include "PORT.h"
#include "RTC.h"
#include "systick.h"
#include "FTM_PWM.h"
#include "ADC.h"
#include "I2C.h"
#include "UART.h"

#define ADDR7BIT(x) ((x) >> 1)

typedef enum
{
	NON = 0,
	RESETUP,
	WAKEUP
} ePowerStatus;
typedef struct CHIP_INFO
{
	U32 IIcCount;
	uint32_t ResetStatus;
	ePowerStatus chipPowerStatus;
	BOOL bForbidCanWakeup;

	uint8_t AccStatus;
	uint32_t Timer1Count;
} CHIP_INFO_STRU;

////////////////////////////////////////////////////////////////////////////////////////
#define PINSEL_PIN_XXX (80)
////////////////////////////////////////////////////////////////////////////////////////

typedef union T123PROGRAMBUILDTIME {
	struct
	{
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t min;
		uint8_t sec;
	} T123ProgramBuildTimeStruct;
	uint8_t T123ProgramBuildTimeBuf[6];
} T123PROGRAMBUILDTIME_UNION;

typedef struct T123RAMINFO
{
	uint8_t T123ProgramVersion[4];
	uint32_t HaveT123ProgramVersion;

	T123PROGRAMBUILDTIME_UNION T123ProgramBuildTime;
	uint32_t HaveT123ProgramBuildTime;

	uint8_t T123FlashSize;
	uint32_t HaveT123FlashSize;
} T123RAMINFO_STRU;

typedef struct
{
	uint8_t ParamIsValid;
	uint8_t bUseSaveParam;
	uint8_t Band;
	uint16_t FreqOfFM;
	uint16_t FreqOfAM;
	uint8_t AreaUserID;
	uint8_t StopSensitivity;
	uint8_t StopMuted;
	uint8_t RDS_AFTA_Enable;
} radio_param_t;

typedef struct
{
	U32 FirstPowerOn1;
	U32 FirstPowerOn2;
	U32 FirstPowerOn3;
	uint32_t WakeUpSrc;

	uint32_t batteryvoltagevalueflag;
	uint16_t batteryvoltagevalue;
	uint32_t havepowerdownAndroid;

	BOOL bOpenLcdStatus;
	T123RAMINFO_STRU T123RamInfo;
	radio_param_t radioInfo;

	uint32_t asyncreset;
	uint32_t PowerManage_StandbyMode;

	uint32_t GPREG0;
	uint32_t GPREG1;
	uint32_t GPREG2;
	uint32_t GPREG3;
	uint32_t GPREG4;
} parameter_info_t;

//************************DEMO IO Macro********
#define DEMOIO_MCU_DEBUG_TXD (GPIO_PTB1) 
#define DEMOIO_MCU_DEBUG_RXD (GPIO_PTB0) 
#define DEMOIO_MCU_TXD2 (GPIO_PTC9)		 
#define DEMOIO_MCU_RXD2 (GPIO_PTC8)		 
#define DEMOIO_MCU_RTC_IN (GPIO_PTA7)	
#define DEMOIO_MCU_SCL (GPIO_PTA3)		 
#define DEMOIO_MCU_SDA (GPIO_PTA2)		 
#define DEMOIO_MCU_SWD_CLK (GPIO_PTC4)   
#define DEMOIO_MCU_SWD_DIO (GPIO_PTA4)   
#define DEMOIO_CAN0_RXD (GPIO_PTC2)		 
#define DEMOIO_CAN0_TXD (GPIO_PTC3)		
#define DEMOIO_MCU_XTAL (GPIO_PTB6) 
#define DEMOIO_MCU_EXTAL (GPIO_PTB7)
#define DEMOIO_CAN0_WAKE (GPIO_PTC7)
#define DEMOIO_CAN1_WAKE (GPIO_PTA0)
#define DEMOIO_CAN2_WAKE (GPIO_PTA1)
#define DEMOIO_INVALID (GPIO_INVALID) 

#define DEMOIO_IIC_C DEMOIO_INVALID

#define DEMOIO_GPIO0 (GPIO_PTB8)
#define DEMOIO_GPIO1 (GPIO_PTB9)
#define DEMOIO_GPIO2 (GPIO_PTB10)
#define DEMOIO_GPIO3 (GPIO_PTB11)
#define DEMOIO_GPIO4 (GPIO_PTD4)
#define DEMOIO_GPIO5 (GPIO_PTB14)
#define DEMOIO_GPIO6 (GPIO_PTB15)
#define DEMOIO_GPIO7 (GPIO_PTB16)
#define DEMOIO_GPIO8 (GPIO_PTB17)
#define DEMOIO_GPIO9 (GPIO_PTA17)
#define DEMOIO_GPIO10 (GPIO_PTB8)
#define DEMOIO_GPIO11 (GPIO_PTB8)

#define DEMOIO_SW0 (GPIO_PTC5)  //switch 4
#define DEMOIO_SW1 (GPIO_PTE0)  //switch 3
#define DEMOIO_SW2 (GPIO_PTE1)  //switch 2
#define DEMOIO_SW3 (GPIO_PTA10) //switch 1

#define HIGH 1
#define LOW 0

#define OLED_POWER (GPIO_PTD12)
#define OLED_CS1 (GPIO_PTD14)
#define OLED_DC (GPIO_PTE8)
#define OLED_SDI (GPIO_PTD16)
#define OLED_CLK (GPIO_PTD15)
#define OLED_CS2 (GPIO_PTD14)
#define OLED_FSO (GPIO_PTD13)

////////////////////////////////////ADC/////////////////////////////////////////////
#define ADC0ConvertBuffrtSize 4
#define ADC1ConvertBuffrtSize 4
#define Y_ADC0 0
#define Y_ADC1 1
#define CHANNEL_ADC(Z, X, Y) ((Z << 16) | (X << 8) | Y) 
#define GET_CHANNEL_ADC_X(Z) Z >> 8
#define GET_CHANNEL_ADC_Y(Z) Z && 0XFF
#define GET_CHANNEL_ADC_Z(Z) Z >> 16

#define CHANNEL_ADC_0 CHANNEL_ADC(0, Y_ADC1, 10) 
#define CHANNEL_ADC_1 CHANNEL_ADC(1, Y_ADC1, 11)					 
#define CHANNEL_ADC_2 CHANNEL_ADC(2, Y_ADC1, 12)			 
#define CHANNEL_ADC_3 CHANNEL_ADC(3, Y_ADC1, 13)	
#define CHANNEL_ADC_4 CHANNEL_ADC(4, Y_ADC1, 4)

////////////////////////////////////////////////////////////////////////////////////

#define PWM_FREQ_100Hz 10000
#define PWM_FREQ_1KHz 1000
#define PWM_FREQ_10KHz 100
#define PWM_FREQ_18KHz 53
#define PWM_FREQ_20KHz 50

#define WDT_1SS 1 //s32k  rtc
#define WDT_1MM (WDT_1SS * 60)
#define WDT_1HH (WDT_1MM * 60)

#define DEBUGMSG 1
#define DEBUG_UART_PORT 0		//���Դ��ڶ˿ں�
#define MCU_DEBUG_TXD GPIO_PTB0 //���Դ��������
#define MCU_DEBUG_RXD GPIO_PTB1 //���Դ��������
#define DEBUG_BAUD 115200		//���Դ��ڲ�����

#define UART2_PORT 2		 //�˿ں�
#define UART2_TXD GPIO_PTE12 //�����
#define UART2_RXD GPIO_PTD17 //�����
#define UART2_BAUD 9600		 //115200		//������

extern parameter_info_t *paramInfo;

void _DI(void);
void _EI(void);
u32 Interface_Get_GPREG(u8 No);
void FlySystemProc(void);
void chipDeInit(void);
void IntoDeepSleep(void);
void CanInit(void);
void carBusInit(void);
void CanIpcEventRegister(void);
u32 CAN_GetESR1Status(u8 Num, U32 Mask);
void carBusDeInit(void);
void CanDeInit(void);
void AllIOConfigInputForSleep(void);
ePowerStatus GetPowerOnBootMode(void);

void carBusIpcEventRegister(void);
void UserResetSystem(void);
void Interface_Init(void);

void SysTick_100ms(void);

void CanProtocolInit(void);
void CanProtocolDeInit(void);
void CanMsgStructInit(CanMsg *msg, uint8_t *buf, uint8_t dlc, uint32_t id, IDE_MODE_ENUM IDEMode);
void CanProtocolRxMsgProc(CanRxMsg *pCAN_RxData);
void Can1ProtocolRxMsgProc(CanRxMsg *pCAN_RxData);

#endif /* LPUART_H_ */
