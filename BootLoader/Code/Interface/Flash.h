
#ifndef __FLASH__H__
#define __FLASH__H__
#include "stdbool.h"
void FlashProgramInit(void);
bool EraseSystemFlashSpace(void);
bool WriteSystemDataToFlash(uint32_t address,uint8_t *pdata,uint16_t len);
#endif 
