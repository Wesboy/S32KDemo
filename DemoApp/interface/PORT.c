

#include "PORT.h"

void PORTC_IRQHandler()
{
	unsigned int val;
	val = PINS_GetPortIntFlag(PORTC);
	PINS_ClearPortIntFlagCmd(PORTC);

	if ((val >> 15) & 0x1)
		paramInfo->WakeUpSrc = 0X01; //ACC
	else if ((val >> 2) & 0x1)
		paramInfo->WakeUpSrc = 0X02; //CAN
}

static void LPGPIO_PinInit(LPGPIO_PinType pin, port_data_direction_t dir, port_pull_config_t pullConfig)
{
	pin_settings_config_t config;
	memset(&config, 0, sizeof(pin_settings_config_t));
	config.mux = PORT_MUX_AS_GPIO;
	switch (pin / 18)
	{
	case 0:
		config.base = PORTA;
		config.gpioBase = PTA;
		break;
	case 1:
		config.base = PORTB;
		config.gpioBase = PTB;
		break;
	case 2:
		config.base = PORTC;
		config.gpioBase = PTC;
		break;
	case 3:
		config.base = PORTD;
		config.gpioBase = PTD;
		break;
	case 4:
		config.base = PORTE;
		config.gpioBase = PTE;
		break;
	}

	config.direction = dir;
	config.pullConfig = pullConfig;
	config.pinPortIdx = pin % 18;
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
	pins_channel_type_t bStatus = FALSE;
	Pin = (LPGPIO_PinType)IOEnum;

	if (bRes && (Pin < GPIO_END))
	{
		unsigned int GPIO[] = GPIO_BASE_ADDRS;
		if (IOEnum == DEMOIO_GPIO2)
		{
			LPGPIO_PinInit(Pin, GPIO_INPUT_DIRECTION, PORT_INTERNAL_PULL_DOWN_ENABLED);
		}
		else
		{
			LPGPIO_PinInit(Pin, GPIO_INPUT_DIRECTION, PORT_INTERNAL_PULL_UP_ENABLED);
		}
		bStatus = (PINS_GPIO_ReadPins(((GPIO_Type *)GPIO[Pin / 18])) >> (Pin % 18)) & 0x1;
	}

	if (FALSE == bStatus)
		return FALSE;
	else
		return TRUE;
}

BOOL IO_Read_No_Pull(unsigned int IOEnum)
{

	LPGPIO_PinType Pin;
	BOOL bRes = TRUE;
	pins_channel_type_t bStatus = FALSE;
	Pin = (LPGPIO_PinType)IOEnum;

	if (bRes && (Pin < GPIO_END))
	{
		unsigned int GPIO[] = GPIO_BASE_ADDRS;
		LPGPIO_PinInit(Pin, GPIO_INPUT_DIRECTION, PORT_INTERNAL_PULL_NOT_ENABLED);
		bStatus = (PINS_GPIO_ReadPins(((GPIO_Type *)GPIO[Pin / 18])) >> (Pin % 18)) & 0x1;
	}
	if (FALSE == bStatus)
		return FALSE;
	else
		return TRUE;
}

BOOL IO_Read_Pull_Down(unsigned int IOEnum)
{

	LPGPIO_PinType Pin;
	BOOL bRes = TRUE;
	pins_channel_type_t bStatus = FALSE;
	Pin = (LPGPIO_PinType)IOEnum;

	if (bRes && (Pin < GPIO_END))
	{
		unsigned int GPIO[] = GPIO_BASE_ADDRS;
		LPGPIO_PinInit(Pin, GPIO_INPUT_DIRECTION, PORT_INTERNAL_PULL_DOWN_ENABLED);
		bStatus = (PINS_GPIO_ReadPins(((GPIO_Type *)GPIO[Pin / 18])) >> (Pin % 18)) & 0x1;
	}

	if (FALSE == bStatus)
		return FALSE;
	else
		return TRUE;
}
/*****************************************************************************
**函数名称:	 	IO_Write
**函数功能:	 	
**入口参数:
**返回参数:
******************************************************************************/
void IO_Write(LPGPIO_PinType IOEnum, bool status)
{
	LPGPIO_PinType Pin;
	BOOL bRes = TRUE;
	Pin = (LPGPIO_PinType)IOEnum;

	if (bRes && (Pin < GPIO_END))
	{
		unsigned int GPIO[] = GPIO_BASE_ADDRS;

		LPGPIO_PinInit(Pin, GPIO_OUTPUT_DIRECTION, PORT_INTERNAL_PULL_DOWN_ENABLED);
		if (status)
		{
			PINS_GPIO_SetPins((GPIO_Type *)GPIO[Pin / 18], 0x1 << (Pin % 18));
		}
		else
		{
			PINS_GPIO_ClearPins((GPIO_Type *)GPIO[Pin / 18], 0x1 << (Pin % 18));
		}
	}
}
