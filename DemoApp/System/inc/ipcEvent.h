#ifndef _IPC_EXCHANGE_H_
#define _IPC_EXCHANGE_H_
#include <stdbool.h>
#include <stdint.h>
#include "fly_types.h"



#define EVT_MODE_MAX_NUM		64

typedef void (*ipcEventSetFunc)(event_t /* evt */, uint32_t  /* param */, uint8_t* /* p */, uint16_t /* len */);
typedef void (*ipcEventGetFunc)(event_t /* evt */, uint32_t* /* param */, uint8_t* /* p */, uint16_t /* len */);


bool ipcEventRegister(uint8_t mode, ipcEventGetFunc ipcEventGet, ipcEventSetFunc ipcEventSet);
void ipcEventGet(event_t evt, uint32_t* param, uint8_t* p, uint16_t len);
void ipcEventSet(event_t evt, uint32_t param, uint8_t* p, uint32_t len);
void ipcEventInit(void);


#endif

