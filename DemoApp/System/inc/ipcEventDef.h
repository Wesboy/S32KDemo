#ifndef __IPC_EVENT_DEF_H__
#define __IPC_EVENT_DEF_H__

#include "fly_types.h"

#define EVT_MODE_GLOBAL					0x00
#define EVT_MODE_ANDROID				0x01
#define EVT_MODE_POWER					0x02
#define EVT_MODE_STATUS         0x03
#define EVT_MODE_KEY            0x04

#define EVT_MODE_DEBUGDEMO          0x18

/* #define EVT_MODE_MAX_NUM		64 --> 0x40 */
#define EVT_MODE_MAX			EVT_MODE_MAX_NUM

/*---------------------------------------------------------------------------*/


#define EVT(_MODE_, _OFFSET_)			(((_MODE_) << 24) + (_OFFSET_))


/* MODE: STATUS GET */
#define EVT_GET_STATUS_MSG_MIN			EVT(EVT_MODE_STATUS, 0x000000)
#define EVT_GET_STATUS_MSG_MAX			EVT(EVT_MODE_STATUS, 0xFFFFFF)

/* MODE: STATUS SET */
#define EVT_SET_STATUS_MSG_MIN			EVT(EVT_MODE_STATUS, 0x000000)
#define EVT_SET_STATUS_MSG_MAX			EVT(EVT_MODE_STATUS, 0xFFFFFF)


/*******************************************************************************/

/* MODE: EVT_MODE_KEY GET */
#define EVT_GET_MODE_KEY_MIN			   EVT(EVT_MODE_KEY, 0x000000)
#define EVT_SET_CAN_KEY		           EVT(EVT_MODE_KEY, 0x000000)
#define EVT_SET_CAN_ABS_KEY          EVT(EVT_MODE_KEY, 0x000001)

#define EVT_GET_MODE_KEY_MAX			   EVT(EVT_MODE_KEY, 0xFFFFFF)



/* MODE: EVT_MODE_KEY SET */

#define EVT_GET_MODE_KEY_MIN			   EVT(EVT_MODE_KEY, 0x000000)
#define EVT_GET_CAN_KEY              EVT(EVT_MODE_KEY, 0x000000)
#define EVT_GET_MODE_KEY_MAX			   EVT(EVT_MODE_KEY, 0xFFFFFF)

/* MODE: CAR_TYPE GET */
#define EVT_GET_CAR_TYPE_MSG_MIN	    EVT(EVT_MODE_CAR_TYPE, 0x000000)

#define EVT_GET_CAR_TYPE_SPECIAL        EVT(EVT_MODE_CAR_TYPE, 0x00000F)

#define EVT_GET_CAR_TYPE_MSG_MAX		EVT(EVT_MODE_CAR_TYPE, 0xFFFFFF)


/* MODE: CAR_TYPE SET */
#define EVT_SET_CAR_TYPE_MSG_MIN		EVT(EVT_MODE_CAR_TYPE, 0x000000)
#define EVT_SET_CAR_TYPE_PROTOCOL       EVT(EVT_MODE_CAR_TYPE, 0x000000)

#define EVT_SET_CAR_TYPE_SPECIAL        EVT(EVT_MODE_CAR_TYPE, 0x00000F)
#define EVT_SET_CAR_TYPE_MSG_MAX		EVT(EVT_MODE_CAR_TYPE, 0xFFFFFF)


/*******************************************************************************/
/* MODE: CAN GET */
#define EVT_GET_CAN_MSG_MIN	                    EVT(EVT_MODE_CAN, 0x000000)
#define EVT_GET_CAN_MSG_MAX		                EVT(EVT_MODE_CAN, 0xFFFFFF)

/* MODE: CAN SET */
#define EVT_SET_CAN_MSG_MIN	                    EVT(EVT_MODE_CAN, 0x000000)
#define EVT_SET_CAN_SEND_DATA_TO_BUS            EVT(EVT_MODE_CAN, 0x000000)
#define EVT_SET_CAN_MSG_MAX		                EVT(EVT_MODE_CAN, 0xFFFFFF)

/*******************************************************************************/

/* MODE: CONFIGFILE GET */
#define EVT_GET_CONFIGFILE_MSG_MIN				EVT(EVT_MODE_CONFIGFILE, 0x000000)
#define EVT_GET_CONFIGFILE_MSG_MAX				EVT(EVT_MODE_CONFIGFILE, 0xFFFFFF)


/* MODE: CONFIGFILE SET */
#define EVT_SET_CONFIGFILE_MSG_MIN				EVT(EVT_MODE_CONFIGFILE, 0x000000)
#define EVT_SET_CONFIGFILE_MSG_MAX				EVT(EVT_MODE_CONFIGFILE, 0xFFFFFF)

/*******************************************************************************/

/* MODE: T123Comm SET */
#define EVT_SET_T123COMMUPDATE_MSG_MIN				EVT(EVT_MODE_T123UPDATE, 0x000000)
#define EVT_SET_T123COMMUPDATE_UPDATE_MSG           EVT(EVT_MODE_T123UPDATE, 0x000000)
#define EVT_SET_T123COMMUPDATE_MSG_MAX				EVT(EVT_MODE_T123UPDATE, 0xFFFFFF)

/*******************************************************************************/
/* MODE: DebugDemo GET */
#define EVT_GET_DEBUGDEMO_MSG_MIN				EVT(EVT_MODE_DEBUGDEMO, 0x000000)
#define EVT_GET_DEBUGDEMO_MSG_MAX				EVT(EVT_MODE_DEBUGDEMO, 0xFFFFFF)

/* MODE: DebugDemo SET */
#define EVT_SET_DEBUGDEMO_MSG_MIN				EVT(EVT_MODE_DEBUGDEMO, 0x000000)
#define EVT_SET_DEBUGDEMO_OSDDISPLAY            EVT(EVT_MODE_DEBUGDEMO, 0x000000)
#define EVT_SET_DEBUGDEMO_OSD_KEY               EVT(EVT_MODE_DEBUGDEMO, 0x000002)
#define EVT_SET_DEBUGDEMO_OSD_IO                EVT(EVT_MODE_DEBUGDEMO, 0x000003)
#define EVT_SET_DEBUGDEMO_OSD_VERSION           EVT(EVT_MODE_DEBUGDEMO, 0x000005)
#define EVT_SET_DEBUGDEMO_MSG_MAX				EVT(EVT_MODE_DEBUGDEMO, 0xFFFFFF)

/*******************************************************************************/
/* MODE: BcmComm SET */
#define EVT_SET_BCM_MSG_MIN				EVT(EVT_MODE_CAR_BCM, 0x000000)
#define EVT_SET_BCM_LIGHT_LOCK_MSG           		EVT(EVT_MODE_CAR_BCM, 0x000000)
#define EVT_SET_BCM_LIGHT_UNLOCK_MSG           	EVT(EVT_MODE_CAR_BCM, 0x000001)
#define EVT_SET_BCM_ALL_STATUS_REQUEST_MSG			EVT(EVT_MODE_CAR_BCM, 0x000002)
#define EVT_SET_BCM_MSG_MAX				EVT(EVT_MODE_CAR_BCM, 0xFFFFFF)

/*******************************************************************************/
#endif

