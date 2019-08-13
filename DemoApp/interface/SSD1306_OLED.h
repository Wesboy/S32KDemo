/************************************************************************************
*  Copyright (c), 2015, HelTec Automation Technology co.,LTD.
*            All rights reserved.
*
* File name: OLED.h
* Project  : HelTec
* Processor: STM32F103C8T6
* Compiler : MDK for ARM - 4.72.1.0
* 
* Author : Aaron.Lee
* Version: 1.01
* Date   : 2014.5.10
* Email  : leehunter8801@gmail.com
* Modification: none
* 
* Description:128*64点阵的OLED显示屏驱动文件，仅适用于惠特自动化(heltec.taobao.com)的SD1306驱动SPI通信方式显示屏
*
* Others: none;
*
* Function List:
*
* 1. void OLED_WrDat(unsigned char dat) -- 向OLED写数据
* 2. void OLED_WrCmd(unsigned char cmd) -- 向OLED写命令
* 3. void OLED_SetPos(unsigned char x, unsigned char y) -- 设置起始点坐标
* 4. void OLED_Fill(unsigned char bmp_dat) -- 全屏填充(0x00可以用于清屏，0xff可以用于全屏点亮)
* 5. void OLED_CLS(void) -- 清屏
* 6. void OLED_Init(void) -- OLED显示屏初始化
* 7. void OLED_6x8Str(unsigned char x, y,unsigned char ch[]) -- 显示6x8的ASCII字符
* 8. void OLED_8x16Str(unsigned char x, y,unsigned char ch[]) -- 显示8x16的ASCII字符
* 9. void OLED_16x16CN(unsigned char x, y, N) -- 显示16x16中文汉字,汉字要先在取模软件中取模
* 10.void OLED_BMP(unsigned char x0, y0,x1, y1,unsigned char BMP[]) -- 全屏显示128*64的BMP图片
*
* History: none;
*
*************************************************************************************/

#ifndef __OLED_H
#define __OLED_H

#include "string.h"
#include "interface.h"

#define Max_Column	128
#define Max_Row			64
#define X_WIDTH 		128
#define Y_WIDTH 		64
#define	Brightness	0xCF 

//-----------------OLED端口定义----------------
#define OLED_CS_Clr() PINS_GPIO_ClearPins(PTB, 0x1<<5)
#define OLED_CS_Set() PINS_GPIO_SetPins(PTB, 0x1<<5)    //接OLED模块CS1（PTB5）

#define OLED_DC_Clr() PINS_GPIO_ClearPins(PTE, 0x1<<8)
#define OLED_DC_Set() PINS_GPIO_SetPins(PTE, 0x1<<8)    //接OLED模块D/C（PTE8）

#define OLED_SDA_Clr() PINS_GPIO_ClearPins(PTD, 0x1<<16)
#define OLED_SDA_Set() PINS_GPIO_SetPins(PTD, 0x1<<16)    //接OLED模块SDI （PTD16）

#define OLED_SCL_Clr() PINS_GPIO_ClearPins(PTD, 0x1<<15)
#define OLED_SCL_Set() PINS_GPIO_SetPins(PTD, 0x1<<15)    //接OLED模块的CLK（PTD15）



//OLED控制用函数
void OLED_WrDat(unsigned char dat);//写数据
void OLED_WrCmd(unsigned char cmd);//写命令
void OLED_SetPos(unsigned char x, unsigned char y);//设置起始点坐标
void OLED_Fill(unsigned char bmp_dat);//全屏填充
void OLED_CLS(void);//清屏
void OLED_Init(void);//初始化

void LcdDisplay_ASCII(uint8_t yPos,uint8_t *GBCodeptr);
void LcdDisChar(uint8_t xPos,uint8_t yPos,uint8_t zknum,uint8_t *zkzip);
uint8_t LcdDisplay_HZ(uint8_t xPos,uint8_t yPos,uint8_t *GBCodeptr);
void LcdDisplay_Chinese(uint8_t xPos,uint8_t yPos,uint8_t *GBCodeptr);
void LcdDisplay_char(uint8_t xPos,uint8_t yPos,uint8_t *GBCodeptr);
void OLED_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_BMP1(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLE_Display_Char(uint8_t *GBCodeptr0,uint8_t *GBCodeptr1,uint8_t *GBCodeptr2);
#endif
