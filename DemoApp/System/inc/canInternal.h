#ifndef __CAN_INTERNAL_H__
#define __CAN_INTERNAL_H__
#include <stdio.h>
#include <stdbool.h>
#include <lpc_types.h>
#include "fly_types.h"

#define FlexCAN0   0
#define FlexCAN1   1
#define FlexCAN2   2

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
void FlexCAN0PinInit(void);
void FlexCAN1PinInit(void);
void FlexCAN2PinInit(void);
void FlyCAN_Transmit(uint32_t CanEnum,CanTxMsg *CAN_Msg);
void FlyCanAddMsg(CanTxMsg* msg);
void FlyCan1AddMsg(CanTxMsg* msg);
void FlyCanSendMsgToCanBus(void);
void FlyCan1SendMsgToCanBus(void);
void CanRxFifoInit(void);
void CanTxFifoInit(void);

void CanProtocolCarUniDevModeInit(void);
void CanProtocolRxMsgProc(CanRxMsg *pCAN_RxData);
void Can1ProtocolRxMsgProc(CanRxMsg *pCAN_RxData);
void CanProtocolInit(void);
void CanProtocolDeInit(void);
void CanProtocolProc(void);
void CanProtocolEventSetProc(event_t evt, uint32_t param, uint8_t *p, uint16_t len);
void CanProtocolEventGetProc(event_t evt, uint32_t* param, uint8_t *p, uint16_t len);
void CanProc(void);
void CanRxFifo_Put_One(CanRxMsg RxData);
void Can1RxFifo_Put_One(CanRxMsg RxData);
void Can2RxFifo_Put_One(CanRxMsg RxData);

#endif

