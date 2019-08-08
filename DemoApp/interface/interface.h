


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

#define ADDR7BIT(x)	 ((x) >> 1)


typedef enum
{
	NON = 0,
	RESETUP,
	WAKEUP
}ePowerStatus;
typedef struct CHIP_INFO
{
   U32 IIcCount;
   uint32_t ResetStatus;
   ePowerStatus chipPowerStatus;
   BOOL bForbidCanWakeup;


   uint8_t AccStatus;
   uint32_t Timer1Count;
}CHIP_INFO_STRU;

////////////////////////////////////////////////////////////////////////////////////////
#define  PINSEL_PIN_XXX  (80)     
////////////////////////////////////////////////////////////////////////////////////////


typedef union T123PROGRAMBUILDTIME
{
    struct
    {
        uint8_t year;   //��2014,ֻȡ14ֵ
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
    }T123ProgramBuildTimeStruct;
	uint8_t T123ProgramBuildTimeBuf[6];
}T123PROGRAMBUILDTIME_UNION;

typedef struct T123RAMINFO
{
	uint8_t T123ProgramVersion[4];
	uint32_t HaveT123ProgramVersion;

    T123PROGRAMBUILDTIME_UNION T123ProgramBuildTime;
	uint32_t HaveT123ProgramBuildTime;

	uint8_t T123FlashSize;
	uint32_t HaveT123FlashSize;
}T123RAMINFO_STRU;

typedef struct {
	uint8_t  ParamIsValid;
	uint8_t  bUseSaveParam;
	uint8_t  Band;
	uint16_t  FreqOfFM;
	uint16_t  FreqOfAM;
	uint8_t  AreaUserID;
	uint8_t  StopSensitivity;
	uint8_t  StopMuted;
	uint8_t  RDS_AFTA_Enable;
}radio_param_t;

typedef struct
{
	U32	FirstPowerOn1;
	U32	FirstPowerOn2;
	U32	FirstPowerOn3;
	uint32_t WakeUpSrc;

    uint32_t batteryvoltagevalueflag;   //��ʾ��ǰ������ػ�ʱ�ĵ�ƿ��ѹֵ�ı�־
	uint16_t batteryvoltagevalue;       //��ʾ����Ĺػ�ʱ�ĵ�ƿ��ѹֵ
	uint32_t havepowerdownAndroid;      //��ʾ�Ƿ��Ѿ��Ͽ���Դ

	BOOL bOpenLcdStatus;                //������ʾ�ϲ�ͨ��2 D X���������ı���״̬X
	T123RAMINFO_STRU T123RamInfo;
	radio_param_t radioInfo;

	uint32_t asyncreset;
	uint32_t PowerManage_StandbyMode;
	
	
	uint32_t  GPREG0;
	uint32_t  GPREG1;
	uint32_t  GPREG2;
	uint32_t  GPREG3;
	uint32_t  GPREG4;
}parameter_info_t;


//*******************���Ŷ���
#define IO_MCU_DEBUG_TXD 			(GPIO_PTB1) 	//���Դ���TX
#define IO_MCU_DEBUG_RXD 			(GPIO_PTB0)		//���Դ���RX
#define IO_MCU_TXD2 					(GPIO_PTC9) 	//cpuͨ�Ŵ���TX
#define IO_MCU_RXD2 					(GPIO_PTC8)		//cpuͨ�Ŵ���RX
#define IO_MCU_RTC_IN    			(GPIO_PTA7)		//MCU��RTCʱ������
#define IO_MCU_SCL						(GPIO_PTA3)//I2Cʱ�� 						
#define IO_MCU_SDA					 	(GPIO_PTA2)//I2C���� 							
#define IO_MCU_SWD_CLK 				(GPIO_PTC4)//SWDʱ�� 									
#define IO_MCU_SWD_DIO 				(GPIO_PTA4)//SWD���� 				
#define IO_CAN0_RXD						(GPIO_PTC2)//CANͨ��RX
#define IO_CAN0_TXD						(GPIO_PTC3)//CANͨ��TX
//#define IO_CAN2_WAKE					(GPIO_PTD16)//can2����									
#define IO_MCU_XTAL						(GPIO_PTB6)//�ⲿ����											
#define IO_MCU_EXTAL					(GPIO_PTB7)//�ⲿ����			
#define IO_LCD_POWER					(GPIO_PTD12)
#define IO_CAN0_WAKE					(GPIO_PTC7)
#define IO_CAN1_WAKE					(GPIO_PTA0)
#define IO_CAN2_WAKE					(GPIO_PTA1)
#define IO_SPI_MCU_SEND_FLAG	(GPIO_PTD14)
#define IO_INVALID 						(GPIO_INVALID)//��Ч��

#define IO_IIC_C							IO_INVALID

#define GPIO0 (GPIO_PTB8)
#define GPIO1 (GPIO_PTB9)
#define GPIO2 (GPIO_PTB10)
#define GPIO3 (GPIO_PTB11)
#define GPIO4 (GPIO_PTD4)
#define GPIO5 (GPIO_PTB14)
#define GPIO6 (GPIO_PTB15)
#define GPIO7 (GPIO_PTB16)
#define GPIO8 (GPIO_PTB17)
#define GPIO9 (GPIO_PTA17)
#define GPIO10 (GPIO_PTB8)
#define GPIO11 (GPIO_PTB8)

#define SW0 (GPIO_PTC5)  //switch 4
#define SW1 (GPIO_PTE0)  //switch 3
#define SW2 (GPIO_PTE1)  //switch 2
#define SW3 (GPIO_PTA10) //switch 1

#define HIGH				1
#define LOW					0


#define OLED_CS1 (GPIO_PTD14)
#define OLED_DC  (GPIO_PTE8)
#define OLED_SDI (GPIO_PTD16)
#define OLED_CLK (GPIO_PTD15)

#define OLED_CS2 (GPIO_PTD14)
#define OLED_FSO (GPIO_PTD14)

#define ADC0ConvertBuffrtSize 4
#define ADC1ConvertBuffrtSize 4
#define Y_ADC0  0
#define Y_ADC1  1
#define CHANNEL_ADC(Z,X,Y)      ((Z<<16)|(X<<8)|Y)//Z:ADCת��ͨ����0~7 ���8�� ����8����Ҫ�޸Ĵ��룩��˳���ţ�PDB��DMA˳��ת���� X��ADC��ţ�0��1�� Y��ADC�ɼ�ͨ��
#define GET_CHANNEL_ADC_X(Z)     Z>>8       
#define GET_CHANNEL_ADC_Y(Z)		 Z&&0XFF
#define GET_CHANNEL_ADC_Z(Z)		 Z>>16

#define 	CHANNEL_ADC_LEFTTURNLAMPERRDETECT	   	CHANNEL_ADC(0,Y_ADC0,15)  //ת���
#define 	CHANNEL_ADC_LIGHT	   									CHANNEL_ADC(1,Y_ADC0,7)  // ��ƿ���
#define 	CHANNEL_ADC_BATVOLDETECT	   					CHANNEL_ADC(2,Y_ADC0,2)  // ���ص�ѹ���
#define 	CHANNEL_ADC_FRONTWIPERSENITYADJ	  	 	CHANNEL_ADC(3,Y_ADC0,8)  // ǰ���

#define 	CHANNEL_ADC_HANDBRAKE_CURRENT	 				CHANNEL_ADC(0,Y_ADC1,10)  // ��ɲ����������
#define 	CHANNEL_ADC_HANDBRAKE		  	 					CHANNEL_ADC(1,Y_ADC1,12)  // ��ɲ��������-��ɲ����
#define 	CHANNEL_ADC_HANDBRAKE_RELEASE		  	 	CHANNEL_ADC(2,Y_ADC1,11)  // ��ɲ��������-��ɲ����



#define KEY_ADC_REF_VOLTAGE		3300    //3300mV
#define KEY_ADC_RESOLUTION		12
/////////////////////////////////////////////////////////////////////////////
//PWM1������1us��ʱ�ķ�ʽ,�������趨��N����ʱ,��ô��Ӧ��PWM1����ΪNus,��Ӧ��Ƶ��Ϊ1/N
//��Ҫ�趨Ƶ��Ϊ18K,��ô��ʱ������N��ֵΪ1/(Nx10^-6)=18000,�õ�N��ֵΪ55
#define		PWM_FREQ_100Hz		10000
#define		PWM_FREQ_1KHz		  1000
#define		PWM_FREQ_10KHz		100
#define		PWM_FREQ_18KHz		53
#define		PWM_FREQ_20KHz		50

#define   WDT_1SS   1    //s32k  rtc 
#define   WDT_1MM   (WDT_1SS*60)
#define   WDT_1HH   (WDT_1MM*60)


#define DEBUGMSG  1
#define	DEBUG_UART_PORT      0          //���Դ��ڶ˿ں�
#define MCU_DEBUG_TXD   GPIO_PTB0 //���Դ��������
#define MCU_DEBUG_RXD   GPIO_PTB1	//���Դ��������
#define DEBUG_BAUD      115200		//���Դ��ڲ�����
			
			
#define	ANDROIDCOMM_UART_PORT	 1
#define ANDROIDCOMM_UART_TXD   GPIO_PTC9 //���Դ��������
#define ANDROIDCOMM_UART_RXD   GPIO_PTC8	//���Դ��������
#define ANDROIDCOMM_UART_BAUD      115200		//���Դ��ڲ�����

#define	UART2_PORT    	  2          //�˿ں�
#define UART2_TXD  			 GPIO_PTE12 //�����
#define UART2_RXD  		 	 GPIO_PTD17	//�����
#define UART2_BAUD    	 9600//115200		//������
					
			
			
#define 		CARBIDICOM_ISUSE  0	
#define CARBIDICOM_UART_PORT	1       
#define CARBIDICOM_UART_BAUD 115200
#define MCU_TXD2 GPIO_PTC8
#define MCU_RXD2 GPIO_PTC9

#define EBoxCom    2
#define EXT_TXD   GPIO_PTE12 //���Դ��������
#define EXT_RXD   GPIO_PTD17	//���Դ��������

// IGNITION LOCK SWITCH IO DEFINE
#define IO_IGNITION_IGN1							GPIO_PTE11  // IGN1--V1
#define IO_IGNITION_IGN2							GPIO_PTB3  // IGN2
#define IO_IGNITION_AM1								GPIO_PTB2  // AM1--INPUT
#define IO_IGNITION_AM2								GPIO_PTC9  // AM2--INPUT
#define IO_IGNITION_ST1								GPIO_PTE7  // ST1--V1
#define IO_IGNITION_ST2								GPIO_PTC15  // ST2
#define IO_IGNITION_ACC								GPIO_PTD11  // ACC--V1

#define MCU_IG1				(GPIO_PTE11) //PTE11,input High active��֧�ֻ��� V1
#define MCU_ST				(GPIO_PTE7)//PTE7,input High active��֧�ֻ��� V1
#define MCU_ACC				(GPIO_PTD11)//PTD11,input High active��֧�ֻ��� V1
#define MCU_F_WIP_AS		(GPIO_PTD16)//PTD16,input Low active
#define MCU_R_WIP_AS		(GPIO_PTA9)//PTA9,input Low active
#define MCU_NINT			(GPIO_PTC11)//PTC11,Device interrupt��֧�ֻ���
#define MCU_R_WIP			(GPIO_PTE0)//
#define MCU_CARGLASS_UP		(GPIO_PTD15)

#define USER_WANT_UPDATE_PARAM			0x5A5A5A5A
				
#define Diag_BMS_Request_ID 0X701
#define Diag_BMS_response_ID 0X702
			

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
u32 CAN_GetESR1Status(u8 Num,U32 Mask);
void carBusDeInit(void);
void CanDeInit(void);
void AllIOConfigInputForSleep(void);
ePowerStatus GetPowerOnBootMode(void);

void carBusIpcEventRegister(void);
void UserResetSystem(void);
void Interface_DebugInit(void);


void SysTick_100ms(void);

void CanProtocolInit(void);
void CanProtocolDeInit(void);
void CanMsgStructInit(CanMsg *msg, uint8_t *buf, uint8_t dlc, uint32_t id, IDE_MODE_ENUM IDEMode);
void CanProtocolRxMsgProc(CanRxMsg *pCAN_RxData);
void Can1ProtocolRxMsgProc(CanRxMsg *pCAN_RxData);

#endif /* LPUART_H_ */





























