

#ifndef __BOOTLOADER__H__
#define __BOOTLOADER__H__

#include "stdbool.h"
typedef struct
{
    uint32_t bUpdateSystemFlag;
	  uint32_t bHaveSystemFlag;
}ImportanceParameter_info_t;
#define IMPORTANCEPARAMETER_ADDRESS    ParameterSection3Ram1Entry
#define IMPORTANCEPARAMETER_SIZE       ParameterSection3Ram1Size     //32B = 0x20
#define USER_WANT2UPDATE_PARA               (0x5a5a5a5a)
#define BIRTHMARKDATA                       (0x0abcdef0)
void CheckPermission(void);
void FlashProgramInit(void);
bool EraseSystemFlashSpace(void);




#endif









