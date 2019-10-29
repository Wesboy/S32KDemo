#include "Interface.h"




#include "interface.h"

void PORTC_IRQHandler()
{

	PINS_ClearPortIntFlagCmd(PORTC);

}

static void LPGPIO_PinInit(LPGPIO_PinType pin,port_data_direction_t dir)
	{
		pin_settings_config_t config;
						memset(&config,0,sizeof(pin_settings_config_t));
					config.mux=PORT_MUX_AS_GPIO;
				switch(pin/18)
				{
					case 0:
						config.base=PORTA;
						config.gpioBase=PTA;
						break;
					case 1:
						config.base=PORTB;
						config.gpioBase=PTB;
						break;
					case 2:
						config.base=PORTC;
						config.gpioBase=PTC;
						break;
					case 3:
						config.base=PORTD;
						config.gpioBase=PTD;
						break;
					case 4:
						config.base=PORTE;
						config.gpioBase=PTE;
						break;
				}
		
					
						config.direction=dir;
						config.pullConfig=PORT_INTERNAL_PULL_DOWN_ENABLED;	
						config.pinPortIdx=pin%18;
						PINS_Init(&config);


	}

/*****************************************************************************
**函数名称:	 	IO_Read
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
BOOL IO_Read(unsigned int IOEnum)
{

	LPGPIO_PinType Pin;
	BOOL bRes = TRUE;
	BOOL bStatus = FALSE;
	Pin=(LPGPIO_PinType)IOEnum;
	
	if(bRes && (Pin < GPIO_END))
	{
		unsigned int GPIO[]= GPIO_BASE_ADDRS;
		LPGPIO_PinInit(Pin,GPIO_INPUT_DIRECTION);
		bStatus=(PINS_GPIO_ReadPins( ((GPIO_Type *)GPIO[Pin/18])) >>(	Pin%18) )&0x1 ;
	}
	return bStatus;
}
/*****************************************************************************
**函数名称:	 	IO_Write
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void IO_Write(unsigned int IOEnum,BOOL status)
{
	LPGPIO_PinType Pin;
	BOOL bRes = TRUE;
	Pin=IOEnum;

	if(bRes && (Pin < GPIO_END))
	{
		unsigned int GPIO[]= GPIO_BASE_ADDRS;
	
		LPGPIO_PinInit(Pin,GPIO_OUTPUT_DIRECTION);
		if(status)
		{
		//	GPIO_PinSet(Pin);
				PINS_GPIO_SetPins((GPIO_Type *)GPIO[Pin/18],0x1<<(Pin%18));
		}
		else
		{
		//	GPIO_PinClear(Pin);
			PINS_GPIO_ClearPins((GPIO_Type *)GPIO[Pin/18],0x1<<(Pin%18));
		}

	
	}
}




















