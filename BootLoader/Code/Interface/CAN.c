#include "interface.h"

NEW_FIFO(CAN0RXBuffer, 50, CanMsg, u16);
NEW_FIFO(CAN0TXBuffer, 50, CanMsg, u16);
NEW_FIFO(CAN1RXBuffer, 50, CanMsg, u16);
NEW_FIFO(CAN1TXBuffer, 50, CanMsg, u16);
NEW_FIFO(CAN2RXBuffer, 50, CanMsg, u16);
NEW_FIFO(CAN2TXBuffer, 50, CanMsg, u16);

static flexcan_msgbuff_t FlexCanRxFifo;

edma_chn_state_t dmaControllerCanRx_State;
edma_chn_state_t dmaControllerCan1Rx_State;
edma_chn_state_t dmaControllerCan2Rx_State;
static flexcan_state_t state = {.busoff = false};
static CAN_Type *const g_flexcanBase[] = CAN_BASE_PTRS;
typedef struct
{
	flexcan_data_info_t tx_info;
	uint8_t Data[8];
	u32 id;
} FlexCAN_MSG_Type;

u32 CANRXbuffer[4] __attribute__((aligned(32)));
u32 CAN1RXbuffer[4] __attribute__((aligned(32)));
u32 CAN2RXbuffer[4] __attribute__((aligned(32)));
/*******************************************************************************************
**函数名称:		CANLPCRxMsg2RxStdMsg
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static void CANLPCRxMsg2RxStdMsg(FlexCAN_MSG_Type *lpc, CanRxMsg *Flystd)
{
	Flystd->DLC = lpc->tx_info.data_length;
	if (lpc->tx_info.msg_id_type == FLEXCAN_MSG_ID_STD)
	{
		Flystd->IDE = CAN_ID_STD;
		Flystd->StdId = lpc->id;
		Flystd->ExtId = 0;
	}
	else
	{
		Flystd->IDE = CAN_ID_EXT;
		Flystd->ExtId = lpc->id;
		Flystd->StdId = 0;
	}
	if (lpc->tx_info.is_remote)
	{

		Flystd->RTR = CAN_RTR_REMOTE;
	}
	else
	{
		Flystd->RTR = CAN_RTR_DATA;
	}
	memcpy(&Flystd->Data[0], lpc->Data, 8);
}
/*******************************************************************************************
**函数名称:		CANTxStdMsg2LPCTxMsg
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static void CANTxStdMsg2LPCTxMsg(CanTxMsg *Flystd, FlexCAN_MSG_Type *lpc)
{
	lpc->tx_info.data_length = Flystd->DLC;
	if (Flystd->IDE == CAN_ID_STD)
	{
		lpc->tx_info.msg_id_type = FLEXCAN_MSG_ID_STD; //卤锚脳录脰隆
		lpc->id = Flystd->StdId;
	}
	else
	{
		lpc->tx_info.msg_id_type = FLEXCAN_MSG_ID_EXT;
		lpc->id = Flystd->ExtId;
	}
	if (Flystd->RTR == CAN_RTR_DATA)
	{
		lpc->tx_info.is_remote = false; //脢媒戮脻脰隆
	}
	else
	{
		lpc->tx_info.is_remote = true;
	}

	memcpy(&lpc->Data[0], &Flystd->Data[0], 8);
}
/*******************************************************************************************
**函数名称:		CAN_GetESR1Status
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static u32 CAN_GetESR1Status(u8 instance, U32 Mask)
{
	DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
	CAN_Type *base = g_flexcanBase[instance];
	return base->ESR1 & Mask;
}
/*******************************************************************************************
**函数名称:		FlyCAN_Transmit
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static void FlyCAN_Transmit(U32 CanEnum, CanTxMsg *CAN_Msg)
{
	FlexCAN_MSG_Type LPC_CANTxMsg;
	flexcan_data_info_t tx_info;
	CANTxStdMsg2LPCTxMsg(CAN_Msg, &LPC_CANTxMsg);

	if (FLEXCAN_DRV_Send(CanEnum, 8, &LPC_CANTxMsg.tx_info, LPC_CANTxMsg.id, LPC_CANTxMsg.Data))
	{
		printf("\r\n CAN_SendMsg  ERROR");
	}
}

/*******************************************************************************************
**函数名称:		FLEXCAN_IRQHandler_callback
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static void(FLEXCAN_IRQHandler_callback)(uint8_t instance, flexcan_event_type_t eventType, uint32_t buffIdx,
										 struct FlexCANState *driverState)
{

	// printf("\r\neventType=%x buffIdx=%x",eventType,buffIdx);
	switch (eventType)
	{
		// extern flexcan_msgbuff_t data2;
	case FLEXCAN_EVENT_RX_COMPLETE: /*!< A frame was received in the configured Rx MB. */
		printf("\r\nFLEXCAN_EVENT_RX_COMPLETE");
		break;
	case FLEXCAN_EVENT_RXFIFO_COMPLETE: /*!< A frame was received in the RxFIFO. */
	{
		CanRxMsg RxData = {
			.IDE = CAN_ID_STD,
			.RTR = CAN_RTR_DATA,
			.FMI = 0,
		};
		RxData.StdId = FlexCanRxFifo.msgId;
		memcpy(RxData.Data, FlexCanRxFifo.data, 8);
		RxData.DLC = FlexCanRxFifo.dataLen;
		//	CanRxFifo_Put_One(RxData);
		printf("\r\n11FLEXCAN_IRQHandler_callback");
	}

	break;
	case FLEXCAN_EVENT_TX_COMPLETE: /*!< A frame was sent from the configured Tx MB. */
		break;
	case FLEXCAN_EVENT_WAKEUP_TIMEOUT: /*!< An wake up event occurred due to timeout. */
		break;
	case FLEXCAN_EVENT_WAKEUP_MATCH: /*!< An wake up event occurred due to matching. */
		break;
	case FLEXCAN_EVENT_SELF_WAKEUP:
		break;
	}
}
/*******************************************************************************************
**函数名称:		FlexCAN0PinInit
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static void FlexCAN0PinInit(void)
{
	pin_settings_config_t config;
	memset(&config, 0, sizeof(pin_settings_config_t));
	config.base = PORTC;
	config.gpioBase = PTC;
	config.mux = PORT_MUX_ALT3;
	config.direction = GPIO_INPUT_DIRECTION;
	config.pullConfig = PORT_INTERNAL_PULL_DOWN_ENABLED;
	config.pinPortIdx = 2;
	config.clearIntFlag = true;
	config.intConfig = PORT_DMA_INT_DISABLED;
	PINS_Init(&config);
	config.direction = GPIO_OUTPUT_DIRECTION;
	config.pinPortIdx = 3;
	PINS_Init(&config);
}

static void FlexCAN1PinInit(void)
{
	pin_settings_config_t config;
	memset(&config, 0, sizeof(pin_settings_config_t));
	config.base = PORTA;
	config.gpioBase = PTA;
	config.mux = PORT_MUX_ALT3;
	config.direction = GPIO_INPUT_DIRECTION;
	config.pullConfig = PORT_INTERNAL_PULL_DOWN_ENABLED;
	config.pinPortIdx = 12;
	config.clearIntFlag = true;
	config.intConfig = PORT_DMA_INT_DISABLED;
	PINS_Init(&config);
	config.direction = GPIO_OUTPUT_DIRECTION;
	config.pinPortIdx = 13;
	PINS_Init(&config);
}

static void FlexCAN2PinInit(void)
{
	pin_settings_config_t config;
	memset(&config, 0, sizeof(pin_settings_config_t));
	config.base = PORTC;
	config.gpioBase = PTC;
	config.mux = PORT_MUX_ALT3;
	config.direction = GPIO_INPUT_DIRECTION;
	config.pullConfig = PORT_INTERNAL_PULL_DOWN_ENABLED;
	config.pinPortIdx = 16;
	config.clearIntFlag = true;
	config.intConfig = PORT_DMA_INT_DISABLED;
	PINS_Init(&config);
	config.direction = GPIO_OUTPUT_DIRECTION;
	config.pinPortIdx = 17;
	PINS_Init(&config);
}

/*******************************************************************************************
**函数名称:		FlexCAN0RxFifoInit
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static inline void FlexCAN0RxFifoInit(void)
{
	flexcan_data_info_t infor = {
		.data_length = 8,
		.fd_enable = false,
		.fd_padding = 0,
		.enable_brs = 0,
		.is_remote = 0,
	};
	u32 idfilter[] = {0};
	flexcan_id_table_t table = {
		.isRemoteFrame = false,   /*!< Remote frame*/
		.isExtendedFrame = false, /*!< Extended frame*/
	};
	table.idFilter = idfilter;
	FLEXCAN_DRV_ConfigRxFifo(0, FLEXCAN_RX_FIFO_ID_FORMAT_A, &table); //配置FIFO rx模式  /*CAN0*/
	FLEXCAN_DRV_SetRxFifoGlobalMask(0, FLEXCAN_MSG_ID_STD, 0);		  //接收所有ID
																	  //	FLEXCAN_DRV_SetRxFifoGlobalMask(0,FLEXCAN_MSG_ID_EXT,0);//接收所有ID
}
/*******************************************************************************************
**函数名称:		FlexCAN1RxFifoInit
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static inline void FlexCAN1RxFifoInit(void)
{
	flexcan_data_info_t infor = {
		.data_length = 8,
		.fd_enable = false,
		.fd_padding = 0,
		.enable_brs = 0,
		.is_remote = 0,
	};
	u32 idfilter[] = {0};
	flexcan_id_table_t table = {
		.isRemoteFrame = false,   /*!< Remote frame*/
		.isExtendedFrame = false, /*!< Extended frame*/
	};
	table.idFilter = idfilter;
	FLEXCAN_DRV_ConfigRxFifo(1, FLEXCAN_RX_FIFO_ID_FORMAT_A, &table); //配置FIFO rx模式  /*CAN0*/
	FLEXCAN_DRV_SetRxFifoGlobalMask(1, FLEXCAN_MSG_ID_STD, 0);		  //接收所有ID
}
/*******************************************************************************************
**函数名称:		FlexCAN2RxFifoInit
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
static inline void FlexCAN2RxFifoInit(void)
{
	flexcan_data_info_t infor = {
		.data_length = 8,
		.fd_enable = false,
		.fd_padding = 0,
		.enable_brs = 0,
		.is_remote = 0,
	};
	u32 idfilter[] = {0};
	flexcan_id_table_t table = {
		.isRemoteFrame = false,   /*!< Remote frame*/
		.isExtendedFrame = false, /*!< Extended frame*/
	};
	table.idFilter = idfilter;
	FLEXCAN_DRV_ConfigRxFifo(2, FLEXCAN_RX_FIFO_ID_FORMAT_A, &table); //配置FIFO rx模式  /*CAN0*/
	FLEXCAN_DRV_SetRxFifoGlobalMask(2, FLEXCAN_MSG_ID_STD, 0);		  //接收所有ID
}

static void(CAN0_DMACALLBACK)(void *parameter, edma_chn_status_t status)
{

	CanRxMsg RxData = {
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_DATA,
		.FMI = 0,
	};
	if ((CANRXbuffer[0] & CAN_CS_IDE_MASK) != 0U)
	{
		RxData.StdId = CANRXbuffer[1];
	}
	else
	{
		RxData.StdId = (CANRXbuffer[1]) >> CAN_ID_STD_SHIFT;
	}
	RxData.DLC = (CANRXbuffer[0] >> 16) & 0xf;
	FlexcanSwapBytesInWord(CANRXbuffer[2], *((u32 *)RxData.Data));
	FlexcanSwapBytesInWord(CANRXbuffer[3], *((u32 *)&RxData.Data[4]));
	//CanRxFifo_Put_One(RxData);

	FIFO_PutOne(CAN0RXBuffer, RxData);
}

static void(CAN1_DMACALLBACK)(void *parameter, edma_chn_status_t status)
{

	CanRxMsg RxData = {
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_DATA,
		.FMI = 0,
	};
	if ((CAN1RXbuffer[0] & CAN_CS_IDE_MASK) != 0U)
	{
		RxData.StdId = CAN1RXbuffer[1];
	}
	else
	{
		RxData.StdId = (CAN1RXbuffer[1]) >> CAN_ID_STD_SHIFT;
	}
	RxData.DLC = (CAN1RXbuffer[0] >> 16) & 0xf;
	FlexcanSwapBytesInWord(CAN1RXbuffer[2], *((u32 *)RxData.Data));
	FlexcanSwapBytesInWord(CAN1RXbuffer[3], *((u32 *)&RxData.Data[4]));

	FIFO_PutOne(CAN1RXBuffer, RxData);
}

static void(CAN2_DMACALLBACK)(void *parameter, edma_chn_status_t status)
{

	CanRxMsg RxData = {
		.IDE = CAN_ID_STD,
		.RTR = CAN_RTR_DATA,
		.FMI = 0,
	};
	if ((CAN2RXbuffer[0] & CAN_CS_IDE_MASK) != 0U)
	{
		RxData.StdId = CAN2RXbuffer[1];
	}
	else
	{
		RxData.StdId = (CAN2RXbuffer[1]) >> CAN_ID_STD_SHIFT;
	}
	RxData.DLC = (CAN2RXbuffer[0] >> 16) & 0xf;
	FlexcanSwapBytesInWord(CAN2RXbuffer[2], *((u32 *)RxData.Data));
	FlexcanSwapBytesInWord(CAN2RXbuffer[3], *((u32 *)&RxData.Data[4]));

	FIFO_PutOne(CAN2RXBuffer, RxData);
}

/*******************************************************************************************
**函数名称:		FlyCAN_Init
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
void FlyCAN_Init(U32 CanEnum, U32 baudrate)
{
	u32 Tq;

	flexcan_state_t state;

	uint32_t SOSCDIV2_freq;
	status_t clk_status = CLOCK_SYS_GetFreq(SOSCDIV2_CLK, &SOSCDIV2_freq); //外设时钟
	Tq = SOSCDIV2_freq / baudrate;										   //Tq

	flexcan_user_config_t cofig =
		{
			.max_num_mb = 16,
			.fd_enable = false,
			//		.is_rx_fifo_needed = false,      //CAN1和CAN2要关闭fifo，不然会死掉
			.is_rx_fifo_needed = true, //使用FIFO接收模式        //CAN0要打开，不然会死掉
			.num_id_filters = FLEXCAN_RX_FIFO_ID_FILTERS_8,
			.flexcanMode = FLEXCAN_NORMAL_MODE,
			.payload = FLEXCAN_PAYLOAD_SIZE_8,
			.pe_clock = FLEXCAN_CLK_SOURCE_SOSCDIV2,
			.transfer_type = FLEXCAN_RXFIFO_USING_DMA,
			.bitrate =
				{
					.phaseSeg1 = 3,
					.phaseSeg2 = 3,
					.propSeg = 6,
					.preDivider = 0,
					.rJumpwidth = 2},
			.rxFifoDMAChannel = DMA_CANFIFORX_CHAN,
		};

	if (CanEnum == FlexCAN0)
	{
		FlexCAN0PinInit();
	}
	else if (CanEnum == FlexCAN1)
	{
		FlexCAN1PinInit();
		cofig.rxFifoDMAChannel = DMA_CAN1FIFORX_CHAN;
	}
	else if (CanEnum == FlexCAN2)
	{
		FlexCAN2PinInit();
		cofig.rxFifoDMAChannel = DMA_CAN2FIFORX_CHAN;
	}

	cofig.bitrate.preDivider = SOSCDIV2_freq / baudrate / (cofig.bitrate.phaseSeg1 + cofig.bitrate.phaseSeg2 + cofig.bitrate.propSeg + 4) - 1;

	printf("\r\nbaudrate=%d Tq:%d phaseSeg1:%d phaseSeg2:%d propSeg:%d", baudrate, Tq, cofig.bitrate.phaseSeg1, cofig.bitrate.phaseSeg2, cofig.bitrate.propSeg);

	state.callback = FLEXCAN_IRQHandler_callback;

	FLEXCAN_DRV_Init(CanEnum, &state, &cofig);

	if (CanEnum == FlexCAN0)
	{
		edma_channel_config_t dmaController1Chn0_Config =
			{
				.channelPriority = EDMA_CHN_DEFAULT_PRIORITY,
				.virtChnConfig = DMA_CANFIFORX_CHAN, //使用DMA通道
				.source = EDMA_REQ_FLEXCAN0,
				.callback = CAN0_DMACALLBACK, //不需要中断
				.callbackParam = NULL};
		DMA__ChannelInit(DMA_CANFIFORX_CHAN, &dmaControllerCanRx_State, &dmaController1Chn0_Config, true);
		DMA_ConfigSingleTransfer(DMA_CANFIFORX_CHAN, EDMA_TRANSFER_MEM2MEM, (uint32_t)&CAN0->RAMn, //配置ADC1传输地址   /*CAN0*/
								 (uint32_t)&CANRXbuffer, EDMA_TRANSFER_SIZE_4B, 16, true);
		FlexCAN0RxFifoInit();
	}
	else if (CanEnum == FlexCAN1)
	{
		edma_channel_config_t dmaController1Chn1_Config =
			{
				.channelPriority = EDMA_CHN_DEFAULT_PRIORITY,
				.virtChnConfig = DMA_CAN1FIFORX_CHAN, //使用DMA通道
				.source = EDMA_REQ_FLEXCAN1,
				.callback = CAN1_DMACALLBACK, //不需要中断
				.callbackParam = NULL};
		DMA__ChannelInit(DMA_CAN1FIFORX_CHAN, &dmaControllerCan1Rx_State, &dmaController1Chn1_Config, true);
		DMA_ConfigSingleTransfer(DMA_CAN1FIFORX_CHAN, EDMA_TRANSFER_MEM2MEM, (uint32_t)&CAN1->RAMn, //配置ADC1传输地址   /*CAN0*/
								 (uint32_t)&CAN1RXbuffer, EDMA_TRANSFER_SIZE_4B, 16, true);
		FlexCAN1RxFifoInit();
	}
	else if (CanEnum == FlexCAN2)
	{
		edma_channel_config_t dmaController1Chn2_Config =
			{
				.channelPriority = EDMA_CHN_DEFAULT_PRIORITY,
				.virtChnConfig = DMA_CAN2FIFORX_CHAN, //使用DMA通道
				.source = EDMA_REQ_FLEXCAN2,
				.callback = CAN2_DMACALLBACK, //不需要中断
				.callbackParam = NULL};
		DMA__ChannelInit(DMA_CAN2FIFORX_CHAN, &dmaControllerCan2Rx_State, &dmaController1Chn2_Config, true);
		DMA_ConfigSingleTransfer(DMA_CAN2FIFORX_CHAN, EDMA_TRANSFER_MEM2MEM, (uint32_t)&CAN2->RAMn, //配置ADC1传输地址   /*CAN0*/
								 (uint32_t)&CAN2RXbuffer, EDMA_TRANSFER_SIZE_4B, 16, true);
		FlexCAN2RxFifoInit();
	}
}

/*******************************************************************************************
**函数名称:		FlyCAN_DeInit
**函数功能:
**入口参数:     无
**返回参数:     无
********************************************************************************************/
void FlyCAN_DeInit(U32 CanEnum)
{
	FLEXCAN_DRV_Deinit(CanEnum);
}

void CanMsgStructInit(CanMsg *msg, uint8_t *buf, uint8_t dlc, uint32_t id, IDE_MODE_ENUM IDEMode)
{
	msg->DLC = dlc;			 //长度
	msg->RTR = CAN_RTR_DATA; //数据帧

	if (IDE_EXT == IDEMode) //扩展帧
	{
		msg->StdId = 0;		   //CAN Std ID
		msg->ExtId = id;	   //CAN Ext ID
		msg->IDE = CAN_ID_EXT; //
	}
	else //标准帧
	{
		msg->StdId = id; //CAN Std ID
		msg->ExtId = 0;  //CAN Ext ID
		msg->IDE = CAN_ID_STD;
	}

	memcpy(msg->Data, buf, 8);
}
void CanProc(void)
{
	if (FIFO_GetCount(CAN0RXBuffer))
	{
		CanMsg RxMsg;
		FIFO_GetOne(CAN0RXBuffer, &RxMsg);
		switch (RxMsg.StdId)
		{
		case Diag_BCM_response_ID:
			Diagnose_Communication_CAN_Receive(RxMsg.Data, RxMsg.DLC);
			break;
		}
	}
	if (FIFO_GetCount(CAN1RXBuffer))
	{
		CanMsg RxMsg;
		FIFO_GetOne(CAN1RXBuffer, &RxMsg);
	}
	if (FIFO_GetCount(CAN2RXBuffer))
	{
		CanMsg RxMsg;
		FIFO_GetOne(CAN2RXBuffer, &RxMsg);
	}
	CAN0SendMsgToCanBus();
	CAN1SendMsgToCanBus();
	CAN2SendMsgToCanBus();
}

static void CAN0SendMsgToCanBus(void)
{
	if (CAN_GetESR1Status(FlexCAN0, CAN_ESR1_TX_MASK) == 0) //传输空闲时传输
		if (FIFO_GetCount(CAN0TXBuffer))
		{
			CanMsg TxMsg;
			FIFO_GetOne(CAN0TXBuffer, &TxMsg);
			FlyCAN_Transmit(FlexCAN0, &TxMsg);
		}
}
static void CAN1SendMsgToCanBus(void)
{
	if (CAN_GetESR1Status(FlexCAN1, CAN_ESR1_TX_MASK) == 0) //传输空闲时传输
		if (FIFO_GetCount(CAN1TXBuffer))
		{
			CanMsg TxMsg;
			FIFO_GetOne(CAN1TXBuffer, &TxMsg);
			FlyCAN_Transmit(FlexCAN1, &TxMsg);
		}
}
static void CAN2SendMsgToCanBus(void)
{
	if (CAN_GetESR1Status(FlexCAN2, CAN_ESR1_TX_MASK) == 0) //传输空闲时传输
		if (FIFO_GetCount(CAN2TXBuffer))
		{
			CanMsg TxMsg;
			FIFO_GetOne(CAN2TXBuffer, &TxMsg);
			FlyCAN_Transmit(FlexCAN2, &TxMsg);
		}
}

void CAN0AddMsg(CanMsg msg)
{
	FIFO_PutOne(CAN0TXBuffer, msg);
}

void CAN1AddMsg(CanMsg msg)
{
	FIFO_PutOne(CAN1TXBuffer, msg);
}

void CAN2AddMsg(CanMsg msg)
{
	FIFO_PutOne(CAN2TXBuffer, msg);
}

void CAN0_Init(u32 baudrate)
{
	FIFO_Init(CAN0RXBuffer);
	FIFO_Init(CAN0TXBuffer);
	FlyCAN_Init(FlexCAN0, baudrate);
	IO_Write(GPIO_PTA0, FALSE);
}

void CAN1_Init(u32 baudrate)
{
	FIFO_Init(CAN1RXBuffer);
	FIFO_Init(CAN1TXBuffer);
	FlyCAN_Init(FlexCAN1, baudrate);
	IO_Write(GPIO_PTA0, FALSE);
}
void CAN2_Init(u32 baudrate)
{
	FIFO_Init(CAN2RXBuffer);
	FIFO_Init(CAN2TXBuffer);
	FlyCAN_Init(FlexCAN2, baudrate);
	IO_Write(GPIO_PTA1, FALSE);
}
