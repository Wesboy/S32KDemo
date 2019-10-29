#ifndef __CAN_FIFO_H__
#define __CAN_FIFO_H__
#define FlexCAN0   0
#define FlexCAN1   1

#define CAN_BUS_BTR_100KHZ		(100000)
#define CAN_BUS_BTR_125KHZ		(125000)
#define CAN_BUS_BTR_500KHZ		(500000)
#define CAN_BUS_BTR_833KHZ		(833000)
#define CAN_BUS_BTR_33333HZ		(33333)
#define CAN_BUS_BTR_83333HZ		(83333)

#define CAN_ID_STD                 ((uint32_t)0x00000000)  /*!< Standard Id */
#define CAN_ID_EXT                 ((uint32_t)0x00000004)  /*!< Extended Id */
#define CAN_RTR_DATA               ((uint32_t)0x00000000)  /*!< Data frame */
#define CAN_RTR_REMOTE             ((uint32_t)0x00000002)  /*!< Remote frame */

#define CarRxCount	256
#define CarTxCount	256

#define NELEMENTS(array)  (sizeof (array) / sizeof (array[0]))
typedef enum IDE_MODE
{
    IDE_STD = 0,
    IDE_EXT,
}IDE_MODE_ENUM;

typedef struct
{
	uint8_t Data[8];
	uint32_t StdId;
	uint32_t ExtId;
	uint8_t IDE;
	uint8_t RTR;
	uint8_t DLC;
	uint8_t FMI;
}CanMsg;

typedef CanMsg CanRxMsg;
typedef CanMsg CanTxMsg;

typedef struct {
	uint8_t  bufferoverflow;
	uint16_t maxsize;
	CanMsg   *buffer;
	CanMsg   *head;
	CanMsg   *tail;
}can_fifo_t;


void FlyCAN_DeInit(uint32_t CanEnum);
void FlyCAN_Init(uint32_t CanEnum,uint32_t baudrate);
void FlyCAN_Transmit(uint32_t CanEnum,CanTxMsg *CAN_Msg);
void FlyCanAddMsg(CanTxMsg msg);

void CanRxFifoInit(void);
void CanTxFifoInit(void);


void CanFifoInit(void);


void CanProc(void);
uint16_t CAN0Tx_FIFO_GetCount(void);
bool CAN0Tx_FIFO_GetOne(CanMsg *pData);
void Can0TxFifo_Put_One(CanTxMsg RxData);
void Can0RxFifo_Put_One(CanRxMsg RxData);
uint16_t CAN0Rx_FIFO_GetCount(void);
bool CAN0Rx_FIFO_GetOne(CanMsg *pData);


#endif 
