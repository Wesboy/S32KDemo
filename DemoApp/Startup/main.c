#include <stdio.h>
#include <string.h>

#include "config.h"
#include "interface.h"


//void LpuartDeInit(unsigned char uart_Nu);
void WatchdogInit(void);

int main(void)
{
	Interface_Init();	
	OLE_Display_Char((u8 *)"Hello!!!",(u8 *)"FlyAudio",(u8 *)"Control_borad");

	while (1)
	{
		FlySystemProc();  
	}	
}

