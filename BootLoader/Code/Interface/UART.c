


#include "Interface.h"
#define	DEBUG_UART_PORT      0          //调试串口端口号
#define MCU_DEBUG_TXD   GPIO_PTB0 //调试串口输出脚
#define MCU_DEBUG_RXD   GPIO_PTB1	//调试串口输入脚
#define DEBUG_BAUD      115200		//调试串口波特率
typedef void (*UART_handler)(uint8_t data);
static void LpuartInit(unsigned char uart_Nu,U32 BAUD,LPGPIO_PinType txPort,LPGPIO_PinType RxPort,UART_handler Handler);
static void  Debug_Handler(u8 data);
static inline void UartPin_Init(LPGPIO_PinType Pin);



static UART_handler UARTHandler[3] = {NULL, NULL, NULL};
void DebugInit(void)
{
	
	LpuartInit(DEBUG_UART_PORT,DEBUG_BAUD,MCU_DEBUG_TXD,MCU_DEBUG_RXD,Debug_Handler);
	
}

/*****************************************************************************
**函数名称:	 	LpuartInit
**函数功能:	 串口功能初始化流程
**入口参数:
**返回参数:
******************************************************************************/
static void LpuartInit(unsigned char uart_Nu,U32 BAUD,LPGPIO_PinType txPort,LPGPIO_PinType RxPort,UART_handler Handler)
{
	
				IRQn_Type LPIrq[]={LPUART0_RxTx_IRQn,LPUART1_RxTx_IRQn,LPUART2_RxTx_IRQn};
					if(uart_Nu>2||txPort>=GPIO_END||RxPort>=GPIO_END)
						return;
						/*串口引脚配置*/
					{
						UartPin_Init(txPort);
						UartPin_Init(RxPort);
					}
				////////////////////////////////////////////////////////////////////
	
				/*串口配置*/
				{
					LPUART_Type * LPUARTx[]= LPUART_BASE_PTRS;
					tlpuart_config UARTCOF;
					UARTCOF.baudRate=BAUD;//波特率
					UARTCOF.bitCountPerChar=LPUART_8_BITS_PER_CHAR;//8位数据位
					UARTCOF.parityMode=LPUART_PARITY_DISABLED;//无奇偶校验
					UARTCOF.stopBitCount=LPUART_ONE_STOP_BIT;//1停止位
					lpuart_device_Init(LPUARTx[uart_Nu],UARTCOF);
					UARTHandler[uart_Nu]=Handler;
					INT_SYS_EnableIRQ(LPIrq[uart_Nu]);
					INT_SYS_SetPriority(LPIrq[uart_Nu], 5);	
			}
			////////////////////////////////////////////////////////////////////
	 
	
}

/*****************************************************************************
**函数名称:	 	UartPin_Init
**函数功能:	 串口引脚初始化配置
**入口参数:
**返回参数:
******************************************************************************/
static inline void UartPin_Init(LPGPIO_PinType Pin)
{
						PORT_Type *Port_PRRS[]=PORT_BASE_PTRS;
						GPIO_Type *Gpio_PTRS[]=GPIO_BASE_PTRS;
						pin_settings_config_t config;
						memset(&config,0,sizeof(pin_settings_config_t));
						config.base=Port_PRRS[Pin/18];
						config.gpioBase=Gpio_PTRS[Pin/18];
					//	config.mux=PORT_MUX_ALT2;
						config.pinPortIdx=Pin%18;
						config.pullConfig=PORT_INTERNAL_PULL_DOWN_ENABLED;	
	
						switch((unsigned char)Pin)
						{
							case GPIO_PTA3:
							case GPIO_PTA2:
								config.mux=PORT_MUX_ALT6;
								break;
							case GPIO_PTB0:
							case GPIO_PTB1:
							case GPIO_PTD7:
							case GPIO_PTD6:		
							case GPIO_PTC8:
							case GPIO_PTC9:
							case GPIO_PTC6:
							case GPIO_PTC7:
								config.mux=PORT_MUX_ALT2;
								break;
							case GPIO_PTE12:
							case GPIO_PTD17:
									config.mux=PORT_MUX_ALT3;
							break;
							default:
								config.mux=PORT_MUX_AS_GPIO;
							return;

						}
						
						PINS_Init(&config);

}



/*****************************************************************
***	函数： fputc
**  功能： 
***	备注： 
******************************************************************/
int fputc(int ch, FILE *f)
{
	
	#define DMADebug 0
	#define   D_LPUARTx       LPUART0

	uint8_t c = (uint8_t)ch& 0xFF;
	uint8_t i;
	char timestr[16];
	int size = 0;

	switch(c)
	{
	case '\n':
		c = '\r';
		#if DMADebug
		FIFO_PUT_ONE(DebugTxFifo,c);
	#else
	lpuart_Putchar(D_LPUARTx, c);
	#endif
		c = '\n';
	#if DMADebug
		FIFO_PUT_ONE(DebugTxFifo,c);
	#else
	lpuart_Putchar(D_LPUARTx, c);
	#endif
		size = snprintf(timestr, 16, "[%10u] ", TimerGetSystemTimer());
		for(i = 0; i < size; i++)
		#if DMADebug
		FIFO_PUT_ONE(DebugTxFifo,timestr[i]);
	#else
	lpuart_Putchar(D_LPUARTx, timestr[i]);
	#endif
		break;
	case '\r':
		break;
	default:
		#if DMADebug
		FIFO_PUT_ONE(DebugTxFifo,c);
	#else
	lpuart_Putchar(D_LPUARTx, c);
	#endif
		break;
	}
	
	

	return 1;
}
/*****************************************************************
***	函数： LPUARTx_IRQHandler
**  功能： 
***	备注： 
******************************************************************/
void LPUARTx_IRQHandler(u8 No)
{
	U8 receive;
	LPUART_Type * LPUARTx[]= LPUART_BASE_PTRS;
	if(No>2)
		return;

	  if((LPUARTx[No]->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT)
		{
			
			  receive = LPUARTx[No]->DATA;            /* Read received data*/
			//	printf("(%c) ",receive);
		}
		if(UARTHandler[No]!=NULL)
		UARTHandler[No](receive);
		if(LPUARTx[No]->STAT&0x80000)
		{
			LPUARTx[No]->STAT|=0x80000;
		}
}
void LPUART0_RxTx_IRQHandler()
{
	
		LPUARTx_IRQHandler(0);
//	printf("\r\nLPUART0_RxTx_IRQHandler");
}
/*****************************************************************
***	函数： LPUART1_RxTx_IRQHandler
**  功能： 
***	备注： 
******************************************************************/
void LPUART1_RxTx_IRQHandler()
{
	  LPUARTx_IRQHandler(1);
	//	printf("\r\nLPUART1_RxTx_IRQHandler");
}
/*****************************************************************
***	函数： LPUART2_RxTx_IRQHandler
**  功能： 
***	备注： 
******************************************************************/
void LPUART2_RxTx_IRQHandler()
{
	   LPUARTx_IRQHandler(2);
}

/*****************************************************************
***	函数： Debug_Handler
**  功能： 
***	备注： 
******************************************************************/
static void  Debug_Handler(u8 data)
{
	//PutAndroidCommRxFifoMsg(data);
}






