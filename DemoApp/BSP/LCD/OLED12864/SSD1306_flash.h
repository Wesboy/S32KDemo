#ifndef _LCD_FLASH_H
#define _LCD_FLASH_H

#include <string.h>
#include "interface.h"

void FF_Init(void);
uint8_t Write_FF(uint8_t value);
void FF_ChipErase(void);
void FF_WriteEnable(void);
uint8_t FF_ReadStatusReg(void);
void FF_WriteStatusReg(uint8_t dat);
void FF_PageWrite(uint8_t *pBuffer, uint32_t addr, uint16_t num);  
void FF_WaitForWriteEnd(void);

void Read_FLASH(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#endif /* end of lcd_flash.h*/
