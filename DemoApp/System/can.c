
#include "interface.h"

#define CAN_FIFO_INIT(__NAME, __ADDR, __NUM) CAN_FIFO_Init(__NAME, __ADDR, __NUM)
#define CAN_FIFO_PUT_ONE_INT(__NAME, __DATA) CAN_FIFO_PutOneInt(__NAME, __DATA)
#define CAN_FIFO_GET_ONE_INT(__NAME, __ADDR) CAN_FIFO_GetOneInt(__NAME, __ADDR)
#define CAN_FIFO_PUT_ONE(__NAME, __DATA) CAN_FIFO_PutOne(__NAME, __DATA)
#define CAN_FIFO_GET_ONE(__NAME, __ADDR) CAN_FIFO_GetOne(__NAME, __ADDR)
#define CAN_FIFO_PEEK_ONE(__NAME, __ADDR, __IDX) CAN_FIFO_PeekOne(__NAME, __ADDR, __IDX)
#define CAN_FIFO_DUMP_DATA(__NAME, __NUM) CAN_FIFO_DumpData(__NAME, __NUM)
#define CAN_FIFO_GET_COUNT(__NAME) CAN_FIFO_GetCount(__NAME)
#define CAN_FIFO_GET_IDEL_COUNT(__NAME) CAN_FIFO_GetIdleCount(__NAME)
#define CAN_FIFO_IS_EMPTY(__NAME) CAN_FIFO_IsEmpty(__NAME)
#define CAN_FIFO_PUT_STR(__NAME, __ADDR, __NUM) CAN_FIFO_PutStr(__NAME, __ADDR, __NUM)
#define CAN_FIFO_GET_STR(__NAME, __ADDR, __NUM) CAN_FIFO_GetStr(__NAME, __ADDR, __NUM)
#define CAN_FIFO_PUTS(__NAME, __ADDR) CAN_FIFO_PutStr(__NAME, __ADDR, sizeof(__ADDR))
#define CAN_FIFO_GET_OVERFLOW(__NAME) CAN_FIFO_GetOverFlow(__NAME)

typedef struct
{
	bool msgOverFlowDisplayFlag;
	CanMsg msg[CarRxCount];
	can_fifo_t CanFifo;
} CarCanRxFifo_t;
typedef struct
{
	bool msgOverFlowDisplayFlag;
	u32 buffer[256][4];
	can_fifo_t CanFifo;
} CarCanRxFifo_t2;
typedef struct
{
	bool msgOverFlowDisplayFlag;
	CanMsg msg[CarTxCount];
	can_fifo_t CanFifo;
} CarCanTxFifo_t;

static CarCanRxFifo_t CarCanRxFifo;
static CarCanTxFifo_t CarCanTxFifo;
static CarCanRxFifo_t CarCan1RxFifo;
static CarCanTxFifo_t CarCan1TxFifo;
static CarCanRxFifo_t CarCan2RxFifo;
static CarCanTxFifo_t CarCan2TxFifo;

#define CanRxFifo (&CarCanRxFifo.CanFifo)
#define CanTxFifo (&CarCanTxFifo.CanFifo)
#define Can1RxFifo (&CarCan1RxFifo.CanFifo)
#define Can1TxFifo (&CarCan1TxFifo.CanFifo)
#define Can2RxFifo (&CarCan2RxFifo.CanFifo)
#define Can2TxFifo (&CarCan2TxFifo.CanFifo)

void CAN_FIFO_Init(can_fifo_t *fifo, CanMsg *buffer, uint16_t size)
{
	_DI();
	fifo->buffer = buffer;
	fifo->maxsize = size;
	fifo->tail = fifo->buffer;
	fifo->head = fifo->buffer;
	fifo->bufferoverflow = 0;
	_EI();
}

uint16_t CAN_FIFO_GetCount(can_fifo_t *fifo)
{
	uint16_t temp;

	_DI();

	if (fifo->head >= fifo->tail)
		temp = fifo->head - fifo->tail;
	else
		temp = fifo->head + fifo->maxsize - fifo->tail;

	_EI();

	return temp;
}

uint16_t CAN_FIFO_GetIdleCount(can_fifo_t *fifo)
{
	uint16_t temp;
	_DI();

	if (fifo->head >= fifo->tail)
		temp = fifo->maxsize - 1 - (fifo->head - fifo->tail);
	else
		temp = fifo->tail - 1 - fifo->head;

	_EI();

	return temp;
}

bool CAN_FIFO_PutOne(can_fifo_t *fifo, CanMsg Data)
{
	uint16_t DataCount;

	_DI();
	if (fifo->head >= fifo->tail)
		DataCount = fifo->head - fifo->tail;
	else
		DataCount = fifo->head + fifo->maxsize - fifo->tail;
	_EI();

	if (DataCount < fifo->maxsize - 1)
	{
		_DI();
		*fifo->head = Data;
		if (fifo->head == fifo->buffer + fifo->maxsize - 1)
			fifo->head = fifo->buffer;
		else
			fifo->head++;
		_EI();

		return TRUE;
	}
	else
		fifo->bufferoverflow = 1;

	return FALSE;
}

bool CAN_FIFO_GetOne(can_fifo_t *fifo, CanMsg *pData)
{
	_DI();
	if (fifo->tail != fifo->head)
	{
		*pData = *fifo->tail;
		if (fifo->tail == fifo->buffer + fifo->maxsize - 1)
			fifo->tail = fifo->buffer;
		else
			fifo->tail++;

		_EI();

		return TRUE;
	}
	_EI();

	return FALSE;
}

bool CAN_FIFO_PeekOne(can_fifo_t *fifo, CanMsg *pData, uint16_t index)
{
	if (index < CAN_FIFO_GetCount(fifo))
	{
		if (fifo->tail + index >= fifo->buffer + fifo->maxsize)
			*pData = *(fifo->tail + index - fifo->maxsize);
		else
			*pData = *(fifo->tail + index);

		return TRUE;
	}

	return FALSE;
}

void CAN_FIFO_DumpData(can_fifo_t *fifo, uint16_t Num)
{
	_DI();
	if (Num < CAN_FIFO_GetCount(fifo))
	{
		fifo->tail += Num;
		if (fifo->tail >= fifo->buffer + fifo->maxsize)
			fifo->tail -= fifo->maxsize;
	}
	else
	{
		fifo->head = fifo->buffer;
		fifo->tail = fifo->buffer;
	}
	_EI();
}

bool CAN_FIFO_PutStr(can_fifo_t *fifo, CanMsg *pData, uint16_t Num)
{
	if ((Num > CAN_FIFO_GetIdleCount(fifo)) || (pData == NULL))
		return FALSE;

	while (Num--)
	{
		_DI();
		*fifo->head = *pData;
		if (fifo->head == fifo->buffer + fifo->maxsize - 1)
			fifo->head = fifo->buffer;
		else
			fifo->head++;
		pData++;
		_EI();
	}

	return TRUE;
}

bool CAN_FIFO_GetStr(can_fifo_t *fifo, CanMsg *pData, uint16_t Num)
{
	if ((Num > CAN_FIFO_GetCount(fifo)) || (pData == NULL))
		return FALSE;

	while (Num--)
	{
		*pData = *fifo->tail;
		_DI();
		if (fifo->tail == fifo->buffer + fifo->maxsize - 1)
			fifo->tail = fifo->buffer;
		else
			fifo->tail++;
		pData++;
		_EI();
	}

	return TRUE;
}

bool CAN_FIFO_IsEmpty(can_fifo_t *fifo)
{
	if (fifo->tail == fifo->head)
		return TRUE;
	else
		return FALSE;
}

void CAN_FIFO_PutOneInt(can_fifo_t *fifo, CanMsg Data)
{
	if (fifo->head + 1 != fifo->tail)
	{
		if ((fifo->head + 1 - fifo->maxsize) != fifo->tail)
		{
			*fifo->head = Data;
			if (fifo->head == fifo->buffer + fifo->maxsize - 1)
				fifo->head = fifo->buffer;
			else
				fifo->head++;
		}
		else //overflow,set flag
			fifo->bufferoverflow = 2;
	}
	else //overflow,set flag
		fifo->bufferoverflow = 3;
}

bool CAN_FIFO_GetOneInt(can_fifo_t *fifo, CanMsg *pData)
{
	if (fifo->tail == fifo->head)
		return FALSE;

	*pData = *fifo->tail;
	if (fifo->tail == fifo->buffer + fifo->maxsize - 1)
		fifo->tail = fifo->buffer;
	else
		fifo->tail++;

	return TRUE;
}

uint8_t CAN_FIFO_GetOverFlow(can_fifo_t *fifo)
{
	return fifo->bufferoverflow;
}

void CanRxFifoInit(void)
{
	CarCanRxFifo.msgOverFlowDisplayFlag = false;
	CAN_FIFO_INIT(CanRxFifo, CarCanRxFifo.msg, NELEMENTS(CarCanRxFifo.msg));
}

void CanTxFifoInit(void)
{
	CarCanTxFifo.msgOverFlowDisplayFlag = false;
	CAN_FIFO_INIT(CanTxFifo, CarCanTxFifo.msg, NELEMENTS(CarCanTxFifo.msg));
}

void Can1RxFifoInit(void)
{
	CarCan1RxFifo.msgOverFlowDisplayFlag = false;
	CAN_FIFO_INIT(Can1RxFifo, CarCan1RxFifo.msg, NELEMENTS(CarCan1RxFifo.msg));
}

void Can1TxFifoInit(void)
{
	CarCan1TxFifo.msgOverFlowDisplayFlag = false;
	CAN_FIFO_INIT(Can1TxFifo, CarCan1TxFifo.msg, NELEMENTS(CarCan1TxFifo.msg));
}

void Can2RxFifoInit(void)
{
	CarCan2RxFifo.msgOverFlowDisplayFlag = false;
	CAN_FIFO_INIT(Can2RxFifo, CarCan2RxFifo.msg, NELEMENTS(CarCan2RxFifo.msg));
}

void Can2TxFifoInit(void)
{
	CarCan2TxFifo.msgOverFlowDisplayFlag = false;
	CAN_FIFO_INIT(Can2TxFifo, CarCan2TxFifo.msg, NELEMENTS(CarCan2TxFifo.msg));
}

void CanFifoInit(void)
{
	CanRxFifoInit();
	CanTxFifoInit();
	Can1RxFifoInit();
	Can1TxFifoInit();
	Can2RxFifoInit();
	Can2TxFifoInit();
}

void CanRxFifo_Put_One(CanRxMsg RxData)
{
	CAN_FIFO_PUT_ONE_INT(CanRxFifo, RxData);
	//	printf("\r\n can0 %d",CAN_FIFO_GET_COUNT(CanRxFifo));
}
void Can1RxFifo_Put_One(CanRxMsg RxData)
{
	CAN_FIFO_PUT_ONE_INT(Can1RxFifo, RxData);
	//	printf("\r\n can1 %d",CAN_FIFO_GET_COUNT(Can1RxFifo));
}
void Can2RxFifo_Put_One(CanRxMsg RxData)
{
	CAN_FIFO_PUT_ONE_INT(Can2RxFifo, RxData);
}

void FlyCanAddMsg(CanTxMsg *msg)
{
	CAN_FIFO_PUT_ONE(CanTxFifo, *msg);
}

void FlyCan1AddMsg(CanTxMsg *msg)
{
	CAN_FIFO_PUT_ONE(Can1TxFifo, *msg);
}

void FlyCan2AddMsg(CanTxMsg *msg)
{
	CAN_FIFO_PUT_ONE(Can2TxFifo, *msg);
}

void FlyCanSendMsgToCanBus(void)
{
	CanMsg mCanMsg;
	if (CAN_GetESR1Status(FlexCAN0, CAN_ESR1_TX_MASK) == 0) //传输空闲时传输
	{
		if (CAN_FIFO_GET_COUNT(CanTxFifo))
		{

			CAN_FIFO_GET_ONE(CanTxFifo, &mCanMsg);
			FlyCAN_Transmit(FlexCAN0, &mCanMsg);
		}
	}
}

void FlyCan1SendMsgToCanBus(void)
{
	CanMsg mCanMsg;
	if (CAN_GetESR1Status(FlexCAN1, CAN_ESR1_TX_MASK) == 0) //传输空闲时传输
	{
		if (CAN_FIFO_GET_COUNT(Can1TxFifo))
		{

			CAN_FIFO_GET_ONE(Can1TxFifo, &mCanMsg);
			FlyCAN_Transmit(FlexCAN1, &mCanMsg);
		}
	}
}

void FlyCan2SendMsgToCanBus(void)
{
	CanMsg mCanMsg;
	if (CAN_GetESR1Status(FlexCAN2, CAN_ESR1_TX_MASK) == 0) //传输空闲时传输
	{
		if (CAN_FIFO_GET_COUNT(Can2TxFifo))
		{

			CAN_FIFO_GET_ONE(Can2TxFifo, &mCanMsg);
			FlyCAN_Transmit(FlexCAN2, &mCanMsg);
		}
	}
}

/**********************************************************************************************************************
**函数名称:	 	ipcEventCanGet
**函数功能:
**入口参数:
**返回参数:
**********************************************************************************************************************/
static void ipcEventCanGet(event_t evt, uint32_t *param, uint8_t *p, uint16_t len)
{
	//	CanProtocolEventGetProc(evt, param, p, len);
}

///**********************************************************************************************************************
//**函数名称:	 	ipcEventCanSet
//**函数功能:
//**入口参数:
//**返回参数:
//**********************************************************************************************************************/
static void ipcEventCanSet(event_t evt, uint32_t param, uint8_t *p, uint16_t len)
{
	switch (evt)
	{

	default:
		break;
	}

	//	CanProtocolEventSetProc(evt, param, p, len);
}

void CanProc(void)
{
	CanMsg mCanMsg;
	static uint32_t HaveCanMsgStatusTimer;
	static uint32_t HaveCan1MsgStatusTimer;

	/************************** CAN0 ***************************/
	if (CAN_FIFO_GetOverFlow(CanRxFifo) && (false == CarCanRxFifo.msgOverFlowDisplayFlag))
	{
		CarCanRxFifo.msgOverFlowDisplayFlag = true;
	}

	if (CAN_FIFO_GET_COUNT(CanRxFifo))
	{
		ResetUserTimer(&HaveCanMsgStatusTimer);
		CAN_FIFO_GET_ONE(CanRxFifo, &mCanMsg);
		CanProtocolRxMsgProc(&mCanMsg);
	}
	else if (ReadUserTimer(&HaveCanMsgStatusTimer) > T_1S * 2)
	{
		//CAN总线超过2S没有接收到消息,就认为车已经休眠下去了
		ResetUserTimer(&HaveCanMsgStatusTimer);
		printf("Not CAN Data\r\n");
	}

	if (CAN_FIFO_GetOverFlow(CanTxFifo) && (false == CarCan1TxFifo.msgOverFlowDisplayFlag))
	{
		CarCanTxFifo.msgOverFlowDisplayFlag = true;
	}

	/************************** CAN1 ***************************/
	if (CAN_FIFO_GetOverFlow(Can1RxFifo) && (false == CarCanRxFifo.msgOverFlowDisplayFlag))
	{
		CarCan1RxFifo.msgOverFlowDisplayFlag = true;
	}

	if (CAN_FIFO_GET_COUNT(Can1RxFifo))
	{
		ResetUserTimer(&HaveCan1MsgStatusTimer);
		CAN_FIFO_GET_ONE(Can1RxFifo, &mCanMsg);
		Can1ProtocolRxMsgProc(&mCanMsg);
	}
	else if (ReadUserTimer(&HaveCan1MsgStatusTimer) > T_1S * 2)
	{
		//CAN总线超过2S没有接收到消息,就认为车已经休眠下去了
		ResetUserTimer(&HaveCan1MsgStatusTimer);
		printf("Not CAN1 Data\r\n");
	}

	if (CAN_FIFO_GetOverFlow(Can1TxFifo) && (false == CarCan1TxFifo.msgOverFlowDisplayFlag))
	{
		CarCan1TxFifo.msgOverFlowDisplayFlag = true;
	}
}

void CanProtocolRxMsgProc(CanRxMsg *pCAN_RxData)
{

#if 1
	printf("\r\n CAN_0->StdId:0x%X ExtID:0x%X", pCAN_RxData->StdId, pCAN_RxData->ExtId);
	for (int i = 0; i < 8; i++)
	{
		printf(" %02x", pCAN_RxData->Data[i]);
	}
#endif
}

void Can1ProtocolRxMsgProc(CanRxMsg *pCAN_RxData)
{

#if 1
	printf("\r\n CAN_1->StdId:0x%X ExtID:0x%X", pCAN_RxData->StdId, pCAN_RxData->ExtId);
	for (int i = 0; i < 8; i++)
	{
		printf(" %02x", pCAN_RxData->Data[i]);
	}

#endif
}

void CanMsgStructInit(CanMsg *msg, uint8_t *buf, uint8_t dlc, uint32_t id, IDE_MODE_ENUM IDEMode)
{
	msg->DLC = dlc;
	msg->RTR = CAN_RTR_DATA;

	if (IDE_EXT == IDEMode)
	{
		msg->StdId = 0;		   //CAN Std ID
		msg->ExtId = id;	   //CAN Ext ID
		msg->IDE = CAN_ID_EXT; //
	}
	else
	{
		msg->StdId = id; //CAN Std ID
		msg->ExtId = 0;  //CAN Ext ID
		msg->IDE = CAN_ID_STD;
	}

	memcpy(msg->Data, buf, 8);
}

void CanDeInit(void)
{
	CanProtocolDeInit();
	CanFifoInit();
}

void CanInit(void)
{
	//All CAN WAKE
	IO_Write(IO_CAN0_WAKE, FALSE); //can0 en
	IO_Write(IO_CAN1_WAKE, FALSE); //can1 en
	IO_Write(IO_CAN2_WAKE, FALSE); //can2 en

	CanFifoInit();
	CanProtocolInit();
}

void CanIpcEventRegister(void)
{
	//ipcEventRegister(EVT_MODE_CAN, ipcEventCanGet, ipcEventCanSet);
}

void CanProtocolInit(void)
{
	printf("\r\n Car Normal init.");
	FlyCAN_Init(FlexCAN0, 500000);
	FlyCAN_Init(FlexCAN1, 500000);
}

void CanProtocolDeInit(void)
{

	FlyCAN_DeInit(FlexCAN0);
}
