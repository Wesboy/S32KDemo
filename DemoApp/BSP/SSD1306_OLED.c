/************************************************************************************
*  Copyright (c), 2015, HelTec Automation Technology co.,LTD.
*            All rights reserved.
*
* File name: OLED.c
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

#include "SSD1306_OLED.h"
#include "SSD1306_flash.h"

#include "interface.h"
#include <stdio.h>
#include <string.h>
#include "codetab.h"

/* SSD1306 commands */
#define SSD1306_SET_MEM_ADDR_MODE    (0x20)

#define SSD1306_SET_COL_ADDR         (0x21)
#define SSD1306_SET_PAGE_ADDR        (0x22)
#define SSD1306_SET_DISP_START_LINE  (0x40)
#define SSD1306_SET_CONTRAST         (0x81)
#define SSD1306_SET_SEGMENT_REMAP0   (0xA0)
#define SSD1306_SET_SEGMENT_REMAP1   (0xA1)
#define SSD1306_SET_ENTIRE_DISP_ON   (0xA5)
#define SSD1306_SET_ENTIRE_DISP_OFF  (0xA4)
#define SSD1306_SET_INVERSION_OFF    (0xA6)
#define SSD1306_SET_INVERSION_ON     (0xA7)

#define SSD1306_SET_MUX_RATIO        (0xA8)
#define SSD1306_MUX_RATIO_MASK       (0x3F)
#define SSD1306_SET_DISPLAY_OFF      (0xAE)
#define SSD1306_SET_DISPLAY_ON       (0xAF)
#define SSD1306_SET_SCAN_DIR_FWD     (0xC0)
#define SSD1306_SET_SCAN_DIR_BWD     (0xC8)
#define SSD1306_SET_DISPLAY_OFFSET   (0xD3)
#define SSD1306_SET_OSC_FREQ         (0xD5)
#define SSD1306_SET_PRE_CHRG_PER     (0xD9)

#define SSD1306_SET_COM_PINS_HW_CFG  (0xDA)
#define SSD1306_COM_PINS_HW_CFG_MASK (0x32)
#define SSD1306_SEQ_COM_PINS_CFG     (0x02)
#define SSD1306_ALT_COM_PINS_CFG     (0x12)
#define SSD1306_COM_LR_REMAP_OFF     (0x02)
#define SSD1306_COM_LR_REMAP_ON      (0x22)

#define SSD1306_SET_DESEL_LVL        (0xDB)
#define SSD1306_SET_NOP              (0xE3)

#define SSD1306_SET_CHARGE_PUMP      (0x8D)
#define SSD1306_CHARGE_PUMP_EN       (0x14)
#define SSD1306_CHARGE_PUMP_DIS      (0x10)

#define SSD1306_SCROLL_HOR_LEFT      (0x27)
#define SSD1306_SCROLL_HOR_RIGHT     (0x26)
#define SSD1306_SCROLL_HOR_VER_LEFT  (0x2A)
#define SSD1306_SCROLL_HOR_VER_RIGHT (0x29)
#define SSD1306_SCROLL_ENABLE        (0x2F)
#define SSD1306_SCROLL_DISABLE       (0x2E)


extern void DelayUs(uint8_t time);

void OLED_WrDat(unsigned char dat) //写数据
{
	unsigned char i;
	OLED_DC_Set();
	DelayUs(10);
	for (i = 0; i < 8; i++)
	{
		if ((dat << i) & 0x80)
		{
			OLED_SDA_Set();
		}
		else
			OLED_SDA_Clr();
		OLED_SCL_Clr();
		DelayUs(10);
		OLED_SCL_Set();
	}
}

void OLED_WrCmd(unsigned char cmd) //写命令
{
	unsigned char i;
	OLED_DC_Clr();
	DelayUs(10);
	for (i = 0; i < 8; i++) //发送一个八位数据
	{
		if ((cmd << i) & 0x80)
		{
			OLED_SDA_Set();
		}
		else
		{
			OLED_SDA_Clr();
		}
		OLED_SCL_Clr();
		DelayUs(10);
		OLED_SCL_Set();
	}
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{
	OLED_WrCmd(0xb0 + y);
	OLED_WrCmd(((x & 0xf0) >> 4) | 0x10);
	OLED_WrCmd((x & 0x0f) | 0x01);
}


void OLED_Fill(unsigned char bmp_dat) //全屏填充
{
	unsigned char y, x;

	OLED_CS_Clr();
	for (y = 0; y < 8; y++)
	{
		OLED_WrCmd(0xb0 + y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for (x = 0; x < X_WIDTH; x++)
		{
			OLED_WrDat(bmp_dat);
		}
	}
	OLED_CS_Set();
}

// iLine < 8
static void OLED_Line_Fill(unsigned char iLine, unsigned char bmp_dat) //全屏填充
{
	if (iLine > 7)
		return;

	unsigned char x;

	OLED_CS_Clr();

	OLED_WrCmd(0xb0 + iLine);
	OLED_WrCmd(0x01);
	OLED_WrCmd(0x10);
	for (x = 0; x < X_WIDTH; x++)
	{
		OLED_WrDat(bmp_dat);
	}
	OLED_CS_Set();
}

void OLED_CLS(void) //清屏
{
	OLED_Fill(0x00);
}

void OLED_Init(void)
{

	IO_Write(OLED_POWER, true);
	Delayms(10);

	IO_Write(OLED_CS1, TRUE);
	IO_Write(OLED_DC, TRUE);
	IO_Write(OLED_SDI, TRUE);
	IO_Write(OLED_CLK, TRUE);

	IO_Write(OLED_CS2, FALSE);

	Delayms(200);

	OLED_CS_Clr();
	OLED_WrCmd(0xae);		//into Sleep Mode
	OLED_WrCmd(0xae);		//--turn off oled panel
	OLED_WrCmd(0x00);		//---set low column address
	OLED_WrCmd(0x10);		//---set high column address
	OLED_WrCmd(0x40);		//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);		//--set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);		//--Set SEG/Column Mapping
	OLED_WrCmd(0xc8);		//Set COM/Row Scan Direction
	OLED_WrCmd(0xa6);		//--set normal display
	OLED_WrCmd(0xa8);		//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3F);		//--1/64 duty
	OLED_WrCmd(0xd3);		//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);		//-not offset
	OLED_WrCmd(0xd5);		//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);		//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);		//--set pre-charge period
	OLED_WrCmd(0xf1);		//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);		//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb); //--set vcomh
	OLED_WrCmd(0x40); //Set VCOM Deselect Level
	OLED_WrCmd(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x00); //
					  //	OLED_WrCmd(0x21);//-Setup column start and end address
					  //	OLED_WrCmd(0x00);//
					  //	OLED_WrCmd(0x7F);//
					  //	OLED_WrCmd(0x22);//-Setup page start and end address
					  //	OLED_WrCmd(0xB7);//
					  //	OLED_WrCmd(0x03);//
	OLED_WrCmd(0x8d); //--set Charge Pump enable/disable
	OLED_WrCmd(0x14); //--set(0x10) disable
	OLED_WrCmd(0xa4); // Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6); // Disable Inverse Display On (0xa6/a7)
	OLED_WrCmd(0xaf); //--turn on oled panel
					 //Into normal mode
	OLED_Fill(0x00);
	OLED_SetPos(0, 0);
	OLED_CS_Set();
}

void LcdDisplay_Chinese(uint8_t xPos, uint8_t yPos, uint8_t *GBCodeptr)
{
	uint8_t i, len;
	len = strlen((const char *)GBCodeptr);
	for (i = 0; i < len; i++)
	{
		LcdDisplay_HZ(xPos + i * 8, yPos, GBCodeptr + i);
		i++;
	}
}

void LcdDisChar(uint8_t xPos, uint8_t yPos, uint8_t zknum, uint8_t *zkzip)
{
	uint8_t i;
	OLED_SetPos(xPos, yPos);
	for (i = 0; i < zknum; i++)
	{
		OLED_WrDat(zkzip[i]);
	}
	OLED_SetPos(xPos, yPos + 1);
	for (i = zknum; i < zknum * 2; i++)
	{
		OLED_WrDat(zkzip[i]);
	}
}

uint8_t LcdDisplay_HZ(uint8_t xPos, uint8_t yPos, uint8_t *GBCodeptr)
{
	uint8_t msb, lsb, zknum;
	uint8_t zkzip[32]; //读取字库数据的缓存区
	uint32_t offset;   //字库地址索引
	uint32_t i;

	OLED_CS_Clr();
	if (xPos >= Max_Column || yPos >= Max_Row)
		return 0;				//超范围退出
	msb = *GBCodeptr;			//汉字或ASCII的机器码的低8位。
	lsb = *(GBCodeptr + 1);		//汉字或ASCII的机器码的高8位。
	if (msb > 128 && lsb > 128) //表明为汉字
	{
		if (xPos + 16 > Max_Column || yPos + 16 > Max_Row)
			return 0;												//超范围退出
		offset = 0x00000 + ((msb - 0xA1) * 94 + (lsb - 0xa1)) * 32; //具体算法详细查看字库原理
		zknum = 16;													//汉字为16*16的字库
	}
	else //否则为ASCII码
	{
		if (xPos + 8 > Max_Column || yPos + 16 > Max_Row)
			return 0;					   //超范围退出
		offset = 0x8100 + (msb - 32) * 16; //查看提供的2012_KZ.txt文档中的“!”的首地址
		zknum = 8;						   // ASCII码位8*16的字库
	}
	Read_FLASH(zkzip, offset, zknum * 2); //从FLASH中读取字库数据。

	printf("\r\n zk data = ");
	for (i = 0; i < sizeof(zkzip); i++)
	{
		printf(" %d", zkzip[i]);
	}
	LcdDisChar(xPos, yPos, zknum, zkzip);
	OLED_CS_Set();

	return 1;
}

uint8_t LcdDisplay_ASCII_MAP(uint8_t *GBCodeptr)
{
	uint8_t data;
	uint32_t i;
	data = *GBCodeptr;
	for (i = 0; i < 8; i++)
	{
		OLED_WrDat(ASCII_MAP[data][i]);
	}
	return 1;
}

void LcdDisplay_ASCII(uint8_t yPos, uint8_t *GBCodeptr)
{
	uint8_t i, len;
	len = strlen((const char *)GBCodeptr);
	OLED_CS_Clr();
	OLED_SetPos(0, yPos);

	if (len > 16)
		len = 16;

	for (i = 0; i < len; i++)
	{
		LcdDisplay_ASCII_MAP(GBCodeptr + i);
	}

	if (len < 16)
	{
		for (i = len; i < 16; i++)
		{
			LcdDisplay_ASCII_MAP(" ");
		}
	}
	OLED_CS_Set();
}
//------------------------------------------------------
//void OLED_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1,unsigned char BMP[])
//在指定区域显示BMP图片
//x0(0~127),yo(0~7) -- 图片起始坐标，x1(1~128),y1(1~8)图片结束点坐标
//将BMP图片导入取模软件生成字模,再将字模放到本工程的codetab.h中
//------------------------------------------------------

void OLED_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	OLED_CS_Clr();
	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_SetPos(x0, y);

		for (x = x0; x < x1; x++)
		{
			OLED_WrDat(BMP[j++]);
		}
	}
	OLED_CS_Set();
}

void OLE_Display_Char(uint8_t *GBCodeptr0, uint8_t *GBCodeptr1, uint8_t *GBCodeptr2)
{
	//OLED_CLS();
	OLED_Line_Fill(7, 0x00);
	Delayms(1);
	LcdDisplay_ASCII(0, GBCodeptr0);
	LcdDisplay_ASCII(1, GBCodeptr1);
	LcdDisplay_ASCII(2, GBCodeptr2);
	LcdDisplay_ASCII(3, "Fuck three Line");
	LcdDisplay_ASCII(4, "what 4");
	LcdDisplay_ASCII(5, "shazi 5");
	LcdDisplay_ASCII(6, "shit 6");
	LcdDisplay_ASCII(7, "test 7");
}
