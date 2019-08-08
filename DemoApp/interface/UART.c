


#include "Interface.h"
#include "lin_driver.h"


#define DebugTxFifoCount 2048

NEW_FIFO(DebugTxFifo,DebugTxFifoCount,u8,u16);


typedef void (*UART_handler)(uint8_t data);


static UART_handler UARTHandler[3] = {NULL, NULL, NULL};
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

/*****************************************************************
***	函数： LpuartDeInit
**  功能： 
***	备注： 
******************************************************************/
static void LpuartDeInit(unsigned char uart_Nu)
{
	LPUART_Type * LPUARTx[]= LPUART_BASE_PTRS;
				IRQn_Type LPIrq[]={LPUART0_RxTx_IRQn,LPUART1_RxTx_IRQn,LPUART2_RxTx_IRQn};
					if(uart_Nu>2)
						return;
					
					lpuart_device_DeInit(LPUARTx[uart_Nu]);
					INT_SYS_DisableIRQ(LPIrq[uart_Nu]);
					UARTHandler[uart_Nu]=NULL;

}
/*****************************************************************
***	函数： Debug_Handler
**  功能： 
***	备注： 
******************************************************************/
static void  Debug_Handler(u8 data)
{
	//收到直接发
	lpuart_Putchar(LPUART0, data);
}
/*****************************************************************************
**函数名称:	 	Debug
**函数功能:	 串口调试初始化接口
**入口参数:
**返回参数:
******************************************************************************/
 void Debug(void)
{
	FIFO_INIT(DebugTxFifo);
	#if(DEBUGMSG)
	LpuartInit(DEBUG_UART_PORT,DEBUG_BAUD,MCU_DEBUG_TXD,MCU_DEBUG_RXD,Debug_Handler);
	#endif
}			


static void  Uart2_Handler(u8 data)
{
//	PutAndroidCommRxFifoMsg(data);
}
	
void Uart2_Init(void)
{
	lin_state_t lin_state;
	lin_user_config_t lin_config;
	
	UartPin_Init(UART2_TXD);
	UartPin_Init(UART2_RXD);
	
	lin_config.baudRate = 19200;
	lin_config.nodeFunction = SLAVE;
	lin_config.autobaudEnable = FALSE;
	
	LIN_DRV_Init(UART2_PORT, &lin_config,  &lin_state);
}		
		
	
	
/*****************************************************************
***	函数： AndroidCommIOInit
**  功能： 
***	备注： 
******************************************************************/
void AndroidCommIOInit()
{
	
	//LpuartInit(ANDROIDCOMM_UART_PORT,ANDROIDCOMM_UART_BAUD,ANDROIDCOMM_UART_TXD,ANDROIDCOMM_UART_RXD,PutAndroidCommRxFifoMsg);
		
}
void AndroidCommIODeInit(void)
{
	//I2C_DeInit(LPC_I2C2);
//	LPI2C_DRV_SlaveDeinit(0);
//	NVIC_DisableIRQ(I2C2_IRQn);
}


/*****************************************************************
***	函数： CurrentDetectComIOInit
**  功能： 
***	备注： 
******************************************************************/
void CurrentDetectComIOInit(void)
{
	//LpuartInit(UART2_PORT,UART2_BAUD,UART2_TXD,UART2_RXD,PutCurrentDetectComRxFifoMsg);
}
void CurrentDetectComIODeInit(void)
{
	//I2C_DeInit(LPC_I2C2);
//	LPI2C_DRV_SlaveDeinit(0);
//	NVIC_DisableIRQ(I2C2_IRQn);
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
/*****************************************************************
***	函数： LPUART0_RxTx_IRQHandler
**  功能： 
***	备注： 
******************************************************************/
void LPUART0_RxTx_IRQHandler()
{
	
		LPUARTx_IRQHandler(0);
}
/*****************************************************************
***	函数： LPUART1_RxTx_IRQHandler
**  功能： 
***	备注： 
******************************************************************/
void LPUART1_RxTx_IRQHandler()
{
	  LPUARTx_IRQHandler(1);
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
#if(DEBUG_UART_PORT==0)
	#define   D_LPUARTx       LPUART0
	#elif(DEBUG_UART_PORT==1)
	#define   D_LPUARTx       LPUART1
	#elif(DEBUG_UART_PORT==2)
	#define   D_LPUARTx       LPUART2
	#endif
/*****************************************************************
***	函数： fputc
**  功能： 
***	备注： 
******************************************************************/
int fputc(int ch, FILE *f)
{
#if(DEBUGMSG==1)
	uint8_t c = (uint8_t)ch& 0xFF;
	uint8_t i;
	char timestr[16];
	int size = 0;

	switch(c)
	{
	case '\n':
		c = '\r';
		lpuart_Putchar(D_LPUARTx, c);
//		FIFO_PUT_ONE(DebugTxFifo,c);
		c = '\n';
		lpuart_Putchar(D_LPUARTx, c);
//		FIFO_PUT_ONE(DebugTxFifo,c);
		size = snprintf(timestr, 16, "[%10u] ", TimerGetSystemTimer());
		for(i = 0; i < size; i++)
		lpuart_Putchar(D_LPUARTx, timestr[i]);
//		FIFO_PUT_ONE(DebugTxFifo,timestr[i]);
		break;
	case '\r':
		break;
	default:
//		FIFO_PUT_ONE(DebugTxFifo,c);
		lpuart_Putchar(D_LPUARTx, c);
		break;
	}
	
	
#endif
	return 1;
}
/*****************************************************************
***	函数： PutAllDebugInfor
**  功能： 取出打印所有串口数据
***	备注： 
******************************************************************/
void PutAllDebugInfor()
{
	u16 Len=FIFO_GET_COUNT(DebugTxFifo);
	for(int i=0;i<Len;i++)
	{
		u8 data;
		 FIFO_GET_ONE(DebugTxFifo, &data);
		while((D_LPUARTx->STAT & LPUART_STAT_TDRE_MASK)==0)
			;
		 D_LPUARTx->DATA=data;
		
		
	}
	printf("\r\n################PutAllDebugInfor##############");
}
/*****************************************************************
***	函数： DebugPro
**  功能： 处理Debug fifo数据
***	备注： 
******************************************************************/
void DebugPro(void)
{
		if(FIFO_GET_COUNT(DebugTxFifo)>DebugTxFifoCount-10)
		{
			PutAllDebugInfor();
		}
	 if((D_LPUARTx->STAT & LPUART_STAT_TDRE_MASK)&&FIFO_GET_COUNT(DebugTxFifo))
	 {
		 u8 data;
		 FIFO_GET_ONE(DebugTxFifo, &data);
		 D_LPUARTx->DATA=data;
	 }
	               /* Send data */
	 

//	 if((LPUART2->STAT & LPUART_STAT_TDRE_MASK)&&FIFO_GET_COUNT(CurrentDetectComTxFifo))
//	 {
//		 u8 data2;
//		// FIFO_GET_ONE(CurrentDetectComTxFifo, &data2);		 
//		 LPUART2->DATA=data2;
//	 }
	 
}

	
 
		
                               
 
	



