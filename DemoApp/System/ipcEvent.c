#include <stdio.h>
#include<string.h>

#include "ipcEventDef.h"
#include "ipcEvent.h"


#ifndef bitvalue
	#define bitvalue(bitx)			((u32)1<<(bitx))
#endif
#ifndef clrbit
	#define clrbit(REG, bitx)	REG &= ~(bitvalue(bitx))
#endif
#ifndef setbit
	#define setbit(REG, bitx)	REG |= (bitvalue(bitx))
#endif

// Check if the bit is '1'
#define bit_is_set(REG, bitx)   (((REG) & bitvalue(bitx)) != 0)
// Check if the bit is '0'
#define bit_is_clr(REG, bitx)   (((REG) & bitvalue(bitx)) == 0)


typedef struct
{
	/* (EVT_MODE_MAX_NUM + 31) / 32  */
	/* Each bit represent one mode to register the call function */
	uint32_t bRegisterEvt[(EVT_MODE_MAX_NUM + 31) >> 5];	
	ipcEventGetFunc ipcEventGetProc[EVT_MODE_MAX_NUM];
	ipcEventSetFunc ipcEventSetProc[EVT_MODE_MAX_NUM];
}ipc_event_info_t;

static ipc_event_info_t gIpcEvtInfo;

extern void ModuleIpcEventRegister(void);

bool ipcEventRegister(uint8_t mode, ipcEventGetFunc ipcEventGet, ipcEventSetFunc ipcEventSet)
{
	if(mode >= EVT_MODE_MAX_NUM) return false;

	setbit(gIpcEvtInfo.bRegisterEvt[mode >> 5], mode & 0x1F); /* mode >> 5 --> mode /32; mode & 0x1F --> mode % 32 */
	gIpcEvtInfo.ipcEventGetProc[mode] = ipcEventGet;
	gIpcEvtInfo.ipcEventSetProc[mode] = ipcEventSet;

	return true;
}

void ipcEventGet(event_t evt, uint32_t* param, uint8_t* p, uint16_t len)
{
	uint8_t mode = (evt >> 24) & 0xFF;

	if(bit_is_set(gIpcEvtInfo.bRegisterEvt[mode >> 5], mode & 0x1F))
	{
		gIpcEvtInfo.ipcEventGetProc[mode](evt, param, p, len);
	}
}

void ipcEventSet(event_t evt, uint32_t param, uint8_t* p, uint32_t len)
{
	int i;
	uint8_t mode = (evt >> 24) & 0xFF;

	switch(mode)
	{
		case 0x00:
			for(i = 1; i < EVT_MODE_MAX_NUM; i++)
			{
				if(bit_is_set(gIpcEvtInfo.bRegisterEvt[i >> 5], i & 0x1F))
				{
					gIpcEvtInfo.ipcEventSetProc[i](evt, param, p, len);
				}
			}
			break;
		default:
			if(bit_is_set(gIpcEvtInfo.bRegisterEvt[mode >> 5], mode & 0x1F))
			{
				gIpcEvtInfo.ipcEventSetProc[mode](evt, param, p, len);
			}
			break;
	}
}

void ipcEventInit(void)
{
	memset(&gIpcEvtInfo, 0, sizeof(ipc_event_info_t));

	ModuleIpcEventRegister();
}


