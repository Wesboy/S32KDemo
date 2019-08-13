#include <stdio.h>
#include <string.h>

#include "interface.h"


//void LpuartDeInit(unsigned char uart_Nu);
void WatchdogInit(void);

int main(void)
{
	Interface_Init();	
	OLE_Display_Char((uint8_t *)"Hello!!!",(uint8_t *)"FlyAudio",(uint8_t *)"Control_borad");

	while (1)
	{
		FlySystemProc();  
	}	
}

