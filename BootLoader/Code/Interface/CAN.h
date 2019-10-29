





#ifndef __CAN__H__
#define __CAN__H__

#include "Def.h"

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

static void CanFifoInit(void);

static void FlyCanAddMsg(CanMsg msg);
static void CAN0SendMsgToCanBus(void);
static void CAN1SendMsgToCanBus(void);
static void CAN2SendMsgToCanBus(void);

extern void CAN0_Init(u32 baudrate);
extern void CAN1_Init(u32 baudrate);
extern void CAN2_Init(u32 baudrate);
extern void CAN0AddMsg(CanMsg msg);
extern void CAN1AddMsg(CanMsg msg);
extern void CAN2AddMsg(CanMsg msg);
extern void CanProc(void);
extern void CanMsgStructInit(CanMsg *msg, uint8_t *buf, uint8_t dlc, uint32_t id, IDE_MODE_ENUM IDEMode);

#endif
















