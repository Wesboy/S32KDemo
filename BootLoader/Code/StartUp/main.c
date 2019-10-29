#include "Interface.h"

int main(void)
{

	static u32 timer = 0;

	BCM_BootloadInit();
	printf("\r\nBootloader BuileTime:%s %s***", __DATE__, __TIME__);
	printf("\r\nreset src %x\r\n", RCM->SRS);
	FlashProgramInit();

	CheckPermission();
#if 1
	printf("\r\nBoot Addr :0x%08x,size:0x%08x", BootloaderProgramAddressEntry, BootloaderProgramSize);
	printf("\r\nParam1Addr:0x%08x,size:0x%08x", ParameterSection1ConfigAddressEntry, ParameterSection1ConfigSize);
	printf("\r\nParam2Addr:0x%08x,size:0x%08x", ParameterSection2ConfigAddressEntry, ParameterSection2ConfigSize);
	printf("\r\nUpdateAddr:0x%08x,size:0x%08x", UpdaterProgramAddressEntry, UpdaterProgramSize);
	printf("\r\nSystemaddr:0x%08x,size:0x%08x", SystemProgramAddressEntry, SystemProgramSize);
#endif

	FlyPDUNetwork_Init(); //诊断通讯初始化
	DMA_Init();
	CAN_Init();
	SysTickInit();
	// WatchdogInit();
	
	while (1)
	{
		BCM_SystemPro();
		if (ReadUserTimer(&timer) > 1000 - 1)
		{
			ResetUserTimer(&timer);

			WDOG_DRV_Trigger(0);
			printf("\r\nBootloader");
		}
	}
}
