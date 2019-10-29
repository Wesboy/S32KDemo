
#include "interface.h"


#define CAN_FIFO_INIT(__NAME, __ADDR, __NUM)		CAN_FIFO_Init(__NAME, __ADDR, __NUM)
#define CAN_FIFO_PUT_ONE_INT(__NAME, __DATA)		CAN_FIFO_PutOneInt(__NAME, __DATA)
#define CAN_FIFO_GET_ONE_INT(__NAME, __ADDR)		CAN_FIFO_GetOneInt(__NAME, __ADDR)
#define CAN_FIFO_PUT_ONE(__NAME, __DATA)			CAN_FIFO_PutOne(__NAME, __DATA)
#define CAN_FIFO_GET_ONE(__NAME, __ADDR)			CAN_FIFO_GetOne(__NAME, __ADDR)
#define CAN_FIFO_PEEK_ONE(__NAME, __ADDR, __IDX)	CAN_FIFO_PeekOne(__NAME, __ADDR, __IDX)
#define CAN_FIFO_DUMP_DATA(__NAME, __NUM)			CAN_FIFO_DumpData(__NAME, __NUM)
#define CAN_FIFO_GET_COUNT(__NAME)					CAN_FIFO_GetCount(__NAME)
#define CAN_FIFO_GET_IDEL_COUNT(__NAME)				CAN_FIFO_GetIdleCount(__NAME)
#define CAN_FIFO_IS_EMPTY(__NAME)					CAN_FIFO_IsEmpty(__NAME)
#define CAN_FIFO_PUT_STR(__NAME, __ADDR, __NUM)		CAN_FIFO_PutStr(__NAME, __ADDR, __NUM)
#define CAN_FIFO_GET_STR(__NAME, __ADDR, __NUM)		CAN_FIFO_GetStr(__NAME, __ADDR, __NUM)
#define CAN_FIFO_PUTS(__NAME, __ADDR)				CAN_FIFO_PutStr(__NAME, __ADDR, sizeof(__ADDR))
#define CAN_FIFO_GET_OVERFLOW(__NAME)				CAN_FIFO_GetOverFlow(__NAME)

typedef struct{
	bool   msgOverFlowDisplayFlag;
	CanMsg msg[CarRxCount];
	can_fifo_t CanFifo;
}CarCanRxFifo_t;

typedef struct{
	bool   msgOverFlowDisplayFlag;
	CanMsg msg[CarTxCount];
	can_fifo_t CanFifo;
}CarCanTxFifo_t;

static CarCanRxFifo_t CarCan0RxFifo;
static CarCanTxFifo_t CarCan0TxFifo;

#define Can0RxFifo	(&CarCan0RxFifo.CanFifo)
#define Can0TxFifo	(&CarCan0TxFifo.CanFifo)



void CAN_FIFO_Init(can_fifo_t* fifo, CanMsg* buffer, uint16_t size)
{
	_DI();
	fifo->buffer = buffer;
	fifo->maxsize = size;
	fifo->tail = fifo->buffer;
	fifo->head = fifo->buffer;
	fifo->bufferoverflow = 0;
	_EI();
	
}

uint16_t CAN_FIFO_GetCount(can_fifo_t* fifo)
{
	uint16_t temp;

	_DI();

	if(fifo->head >= fifo->tail)
		temp = fifo->head - fifo->tail;
	else
		temp = fifo->head + fifo->maxsize - fifo->tail;

	_EI();

	return temp;
}

uint16_t CAN_FIFO_GetIdleCount(can_fifo_t* fifo)
{
	uint16_t temp;
	_DI();

	if(fifo->head >= fifo->tail)
		temp = fifo->maxsize - 1 - (fifo->head - fifo->tail);
	else
		temp = fifo->tail - 1 - fifo->head;

	_EI();

	return temp;
}

bool CAN_FIFO_PutOne(can_fifo_t* fifo, CanMsg Data)
{
	uint16_t DataCount;

	_DI();
	if(fifo->head >= fifo->tail)
		DataCount = fifo->head - fifo->tail;
	else
		DataCount = fifo->head + fifo->maxsize - fifo->tail;
	_EI();

	if(DataCount < fifo->maxsize - 1)
	{
		_DI();
		*fifo->head = Data;
		if(fifo->head == fifo->buffer + fifo->maxsize - 1)
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

bool CAN_FIFO_GetOne(can_fifo_t* fifo, CanMsg *pData)
{
	_DI();
	if(fifo->tail != fifo->head)
	{
		*pData = *fifo->tail;
		if(fifo->tail == fifo->buffer+fifo->maxsize - 1)
			fifo->tail = fifo->buffer;
		else
			fifo->tail++;

		_EI();

		return TRUE;
	}
	_EI();

	return FALSE;
}

bool CAN_FIFO_PeekOne(can_fifo_t* fifo, CanMsg *pData, uint16_t index)
{
	if(index < CAN_FIFO_GetCount(fifo))
	{
		if(fifo->tail + index >= fifo->buffer + fifo->maxsize)
			*pData = *(fifo->tail+index-fifo->maxsize);
		else
			*pData = *(fifo->tail+index);

		return TRUE;
	}

	return FALSE;
}

void CAN_FIFO_DumpData(can_fifo_t* fifo, uint16_t Num)
{
	_DI();
	if(Num < CAN_FIFO_GetCount(fifo))
	{
		fifo->tail += Num;
		if(fifo->tail >= fifo->buffer + fifo->maxsize)
			fifo->tail -= fifo->maxsize;
	}
	else
	{
		fifo->head = fifo->buffer;
		fifo->tail = fifo->buffer;
	}
	_EI();
}

bool CAN_FIFO_PutStr(can_fifo_t* fifo, CanMsg *pData, uint16_t Num)
{
	if((Num > CAN_FIFO_GetIdleCount(fifo)) || (pData == NULL))
		return FALSE;

	while(Num--)
	{
		_DI();
		*fifo->head = *pData;
		if(fifo->head == fifo->buffer + fifo->maxsize - 1)
			fifo->head = fifo->buffer;
		else
			fifo->head++;
		pData++;
		_EI();
	}

	return TRUE;
}

bool CAN_FIFO_GetStr(can_fifo_t* fifo, CanMsg *pData, uint16_t Num)
{
	if((Num > CAN_FIFO_GetCount(fifo)) || (pData == NULL))
		return FALSE;

	while(Num--)
	{
		*pData = *fifo->tail;
		_DI();
		if(fifo->tail == fifo->buffer + fifo->maxsize - 1)
			fifo->tail = fifo->buffer;
		else
			fifo->tail++;
		pData++;
		_EI();
	}

	return TRUE;
}

bool CAN_FIFO_IsEmpty(can_fifo_t* fifo)
{
	if(fifo->tail == fifo->head)
		return TRUE;
	else
		return FALSE;
}

void CAN_FIFO_PutOneInt(can_fifo_t* fifo, CanMsg Data)
{
	if(fifo->head+1 != fifo->tail)
	{
		if((fifo->head+1-fifo->maxsize) != fifo->tail)
		{
			*fifo->head = Data;
			if(fifo->head == fifo->buffer+fifo->maxsize-1)
				fifo->head = fifo->buffer;
			else
				fifo->head++;
		}
		else    //overflow,set flag
			fifo->bufferoverflow = 2;
	}
	else    //overflow,set flag
		fifo->bufferoverflow = 3;
}

bool CAN_FIFO_GetOneInt(can_fifo_t* fifo, CanMsg *pData)
{
	if(fifo->tail == fifo->head) return FALSE;

	*pData = *fifo->tail;
	if(fifo->tail == fifo->buffer+fifo->maxsize-1)
		fifo->tail = fifo->buffer;
	else
		fifo->tail++;

	return TRUE;
}

uint8_t  CAN_FIFO_GetOverFlow(can_fifo_t* fifo)
{
	return fifo->bufferoverflow;
}


void CanRxFifoInit(void)
{
	CAN_FIFO_INIT(Can0RxFifo, CarCan0RxFifo.msg, NELEMENTS(CarCan0RxFifo.msg));
}

void CanTxFifoInit(void)
{
	CAN_FIFO_INIT(Can0TxFifo, CarCan0TxFifo.msg, NELEMENTS(CarCan0TxFifo.msg));
}

void CanFifoInit(void)
{
	CanRxFifoInit();
	CanTxFifoInit();

}

void CanRxFifo_Put_One(CanRxMsg RxData)
{
	
	CAN_FIFO_PUT_ONE_INT(Can0RxFifo,RxData);
}
void Can0RxFifo_Put_One(CanRxMsg RxData)
{
	CAN_FIFO_PUT_ONE_INT(Can0RxFifo,RxData);
}
void Can0TxFifo_Put_One(CanTxMsg RxData)
{
	CAN_FIFO_PUT_ONE_INT(Can0TxFifo,RxData);
}

uint16_t CAN0Tx_FIFO_GetCount()
{
	return CAN_FIFO_GetCount(Can0TxFifo);
}
uint16_t CAN0Rx_FIFO_GetCount()
{
	return CAN_FIFO_GetCount(Can0RxFifo);
}
bool CAN0Rx_FIFO_GetOne(CanMsg *pData)
{
	return CAN_FIFO_GetOne(Can0RxFifo,pData);
}
bool CAN0Tx_FIFO_GetOne(CanMsg *pData)
{
	return CAN_FIFO_GetOne(Can0TxFifo,pData);
}

