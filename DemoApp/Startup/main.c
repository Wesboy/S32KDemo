#include <stdio.h>
#include <string.h>

#include "config.h"
#include "ipcEvent.h"
#include "ipcEventDef.h"
#include "interface.h"

//#include "codetab.h"

#if defined(HEX_DEBUG)
#define SysGuideCfgAddressEntry ParameterSection2FlashEntry// 0x00005200
const unsigned CRP_VERSION1 __attribute__((at(SysGuideCfgAddressEntry))) = 0x12345678;
const unsigned CRP_VERSION2 __attribute__((at(SysGuideCfgAddressEntry+0X04))) = 0x87654321;
#endif

 //void LpuartDeInit(unsigned char uart_Nu);
 void WatchdogInit(void);
extern unsigned char testbmp[][31];
int main(void)
{
	Interface_DebugInit();
	printf("\r\nLPC BuileTime:%s %s",__DATE__,__TIME__);

	IO_Write(IO_LCD_POWER,true);
	IO_Write(IO_CAN0_WAKE,FALSE); //can0 en
	IO_Write(IO_CAN1_WAKE,FALSE); //can1 en
	IO_Write(IO_CAN2_WAKE,FALSE); //can2 en
	
	Delayms(10);
	OLED_Init();
	FF_Init();
	OLED_Fill(0);
	Delayms(10);	
	OLE_Display_Char((u8 *)"Hello!!!",(u8 *)"FlyAudio",(u8 *)"Control_borad");


	while (1)
	{
		FlySystemProc();  
	}	
}

