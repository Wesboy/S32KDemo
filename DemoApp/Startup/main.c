#include <stdio.h>
#include <string.h>

#include "interface.h"


#define SysGuideCfgAddressEntry ParameterSection2FlashEntry// 0x00005200
const unsigned CRP_VERSION1 __attribute__((at(SysGuideCfgAddressEntry))) = 0x12345678;
const unsigned CRP_VERSION2 __attribute__((at(SysGuideCfgAddressEntry+0X04))) = 0x87654321;

//void LpuartDeInit(unsigned char uart_Nu);
void WatchdogInit(void);

int main(void)
{
	uint32_t iMainTimer;
	Interface_Init();	
	
	OLED_CLS();
	LcdDisplay_Chinese(0, 0, (uint8_t *)"chen");

	while (1)
	{
		if(ReadUserTimer(&iMainTimer) > 300)
		{
			ResetUserTimer(&iMainTimer);
//			OLE_Display_Char((uint8_t *)"Hello!!!",(uint8_t *)"FlyAudio",(uint8_t *)"Control_borad");
		}
		FlySystemProc();  
	}	
}

