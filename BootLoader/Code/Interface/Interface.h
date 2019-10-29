


#ifndef __INTERFACE__H__
#define __INTERFACE__H__

#include "stdbool.h"
#include "Def.h"
#include "UART.h"
#include "PORT.h"
#include "CAN.h"
#include "Fifo.h"
#include "Flash.h"
#include "S32K144/include/S32K144.h"

#include "pins_port_hw_access.h"
#include "pins_gpio_hw_access.h"
#include "wdog_driver.h"
#include "clock_manager.h"
#include "flexcan_driver.h"
#include "flexcan_hw_access.h"


#include "MemoryConfig.h"
#include "s32k144_lpuart.h"
#include "interrupt_manager.h"
#include "stdio.h"
#include "string.h"
#include "Systick.h"
#include "WatchDog.h"
#include "DMA.h"
//#include "CAN_Fifo.h"
#include "FlyPDUNetwork.h"

#include "programming.h"
#include "flash_driver.h"
#include "Bootloader.h"

#define DMA_ADC0_CHAN 0
#define DMA_ADC1_CHAN 1
#define DMA_CANFIFORX_CHAN 2
#define DMA_CAN1FIFORX_CHAN 3
#define DMA_CAN2FIFORX_CHAN 4

#define Diag_BCM_Request_ID 0X703
#define Diag_BCM_response_ID 0X704

void BCM_BootloadInit(void);
void BCM_SystemPro(void);
void CAN_Init(void);
void GoT0WhatStatus(uint32_t Address);
void _DI(void);
void _EI(void);
void Programming_SetUpdataSystemFlag(void);
void Programming_ClearUpdataSystemFlag(void);
u32 Bootloader_GetUpdataSystemFlag(void);
u32 Bootloader_GetHaveSystemFlag(void);
void Diagnose_Communication_CAN_Send(unsigned char *buff, unsigned char dlc);

#endif

