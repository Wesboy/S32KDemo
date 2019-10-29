/* programming.c                              Flyaudio 2019
 * 描述: BCM Bootloader的升级模块
 *              for S32K144
 * 2019 02 20 移植预研阶段的诊断升级模块
 * 
 * 
 */
#include "programming.h"
#include "Servce.h"
#include "def.h"
#include "Flash.h"  
#include "Diagnose_Communication.h"
#include "Systick.h"
#include "stdio.h"
#include "MemoryConfig.h"
#include "Bootloader.h"
#include "string.h"
#define  eraRoutineDID 0x1111
#define  BOOTRoutineDID 0x2222
#define  USERRoutineDID 0x3333
#define  CheckisBootDID 0x4444
#define  SetSystemFlagDID 0x5555
#define  DownloadmaxNumberOfBlockLength 16

unsigned char RTSession=fuc_defaultSession;
unsigned int  SessionResetTime=0;
	
unsigned char RTDTC=fuc_eDTCSettingOff;
unsigned int RoutineDelaytime=0;
unsigned int T0WhatStatusAdder;

void Programming_SetUpdataSystemFlag(void);
void Programming_ClearUpdataSystemFlag(void);
u32 Bootloader_GetUpdataSystemFlag(void);
typedef struct 
{
	u8  DownloadBuff[4096];
	u32 DownloadAdderStart;//下载起始地址
	u32 DownloadAdderSize;//下载地址的大小
	unsigned long DownloadorUploadAdder;//当前下载或上传的地址
	u8 	BlockNo;//块校对
	u8  Type;//下载或者上传
	
	u16  DownloadbufVail;
}__DownloadService;
static __DownloadService  gDownloadService;
void GoT0WhatStatus(uint32_t Address);

/***************************
***	名称：	Programming_Init
***	描述： 
*** 备注：  暂时不用 
****************************/
void Programming_Init(void)
{
	
}


/***************************
			BootorUser
		判断是否跳转到User
****************************/
void 	BootorUser()
{
//	if(Bkp_ReadBootMode()!=0x1111)
//	{
//			printf("\r\nBootorUser1 %04x",*(u32*)(UserProgramAddressStart));
//			if(*(u32*)(UserProgramAddressStart)==0x12345678)
//			{
//				
//				T0WhatStatusAdder=UserProgramAddressEntry;
//				RoutineDelaytime=1;
//			}
//	}
}


/***************************
			ProgramPro
		
****************************/
void ProgramPro(void)//
{
		/* 接受诊断数据后5S若没有接受到其他数据，跳回SYSTEM*/
			if(ReadUserTimer(&SessionResetTime)>5000&&SessionResetTime!=0)
			{
				Programming_ClearUpdataSystemFlag();
		    CheckPermission();//当有程序时跳到SYSTEM
				GoT0WhatStatus(BootloaderProgramAddressEntry);//没有程序就跳回bootloader
			}
			/*延时500ms再执行地址跳转*/
			if(ReadUserTimer(&RoutineDelaytime)>500&&RoutineDelaytime!=0)
			{
			//	chipDeInit();
				RoutineDelaytime=0;
		   GoT0WhatStatus(T0WhatStatusAdder);
			}
}
void ClaerSessionTime(void)
{
	if(SessionResetTime!=0)
	ResetUserTimer(&SessionResetTime);
}
/***************************
			erasureRoutine
			USER地址擦除例程
****************************/
void  erasureRoutine()//用户地址全擦除
{
//	INT32U re0;
////	re0=sectorPrepare(APP_START_SECTOR, APP_END_SECTOR);
//	if(re0)
//		printf("\r\nerasureRoutine:sectorPrepare");
////	re0=sectorErase(APP_START_SECTOR, APP_END_SECTOR);
//	if(re0)
//		printf("\r\nerasureRoutine:sectorPrepare");

}
/***************************
			Prog_SetDiagSession
			会话设置
****************************/
void Prog_SetDiagSession(unsigned char Session)
{
	RTSession=Session;
	ResetUserTimer(&SessionResetTime);
}
/***************************
			Prog_SetDTC
				DTC设置
****************************/
void Prog_SetDTC(unsigned char DTC)
{
	RTDTC=DTC;
}
/***************************
			Prog_CommunicationControl
			通信使能停止控制
****************************/
void Prog_CommunicationControl(unsigned char Communication)
{
			

}
/***************************
			BOOTRoutine
			BOOT跳转例程
****************************/
void 	BOOTRoutine()//跳转到BOOT地址
{
  	printf("\r\nBOOTRoutine");
  	ResetUserTimer(&RoutineDelaytime);
	 
  	T0WhatStatusAdder=BootloaderProgramAddressEntry;
}
/***************************
			USERRoutine
			USER跳转例程
****************************/
void USERRoutine()//跳转到USER地址
{
	printf("\r\nUSERRoutine");
	ResetUserTimer(&RoutineDelaytime);
	
	T0WhatStatusAdder=SystemProgramAddressEntry;
}
/***************************
			Prog_RoutineControl
			例程控制
****************************/
unsigned char Prog_RoutineControl(unsigned char Routine,unsigned short int DID)
{//例程控制
	switch(Routine)
	{ 
		case fuc_startRoutine:
			switch(DID)
			{
				case eraRoutineDID://擦除例程
				{
					bool ret;
					Programming_ClearHaveSystemFlag();
					ret= EraseSystemFlashSpace();
					printf("\r\nbl--Erase System Flash ");
					if(ret==true)
					{
						
						printf("ok");
					}
					else
					{
						printf("failure");
					}
						
				}
					
					break;
				case BOOTRoutineDID://跳转BOOT例程
					printf("\r\nbl--boot to bootloader ");
					 Programming_SetUpdataSystemFlag();
					BOOTRoutine();
					break;
				case USERRoutineDID://跳转USER例程
					printf("\r\nbl--boot to system ");
					Programming_ClearUpdataSystemFlag();
					USERRoutine();
					break;
				case CheckisBootDID://
					printf("\r\nbl--checkin bootloader ");
					break;
				case SetSystemFlagDID://
						{
						printf("\r\nbl--set flag ");
            Programming_SetHaveSystemFlag();
							//	printf("Bootloader_GetHaveSystemFlag()=%x,",Bootloader_GetHaveSystemFlag());
						}
					break;
			
				
				default :
					printf("\r\nbl--DID=%x",DID);
					return NRC_0x31_ROOR;
			}
		break;
		case fuc_stopRoutine:
			break;
		case fuc_requestRoutineResults:
			break;
	}
		return 0;
}
/***************************
	Prog_RequestDownloadservice
	   请求下载服务
****************************/

unsigned char Prog_RequestDownloadservice(unsigned char *buff,unsigned char len,unsigned char *Para)
{
	unsigned char i;
	if((buff[0]&0xf)+(buff[0]>>4)!=len-1)
	return NRC_0x31_ROOR;
	gDownloadService.DownloadAdderStart=0;
	gDownloadService.DownloadAdderSize=0;
	for(i=0;i<(buff[0]>>4);i++)
	{
		gDownloadService.DownloadAdderStart=gDownloadService.DownloadAdderStart<<8;
		gDownloadService.DownloadAdderStart+=buff[i+1];
	}
	for(;i<(buff[0]&0xf)+(buff[0]>>4);i++)
	{
		gDownloadService.DownloadAdderSize=gDownloadService.DownloadAdderSize<<8;
		gDownloadService.DownloadAdderSize+=buff[i+1];
	}
	printf("\r\nDownloadAdderStart=%x DownloadAdderSize=%d",gDownloadService.DownloadAdderStart,gDownloadService.DownloadAdderSize);
	//if(gDownloadService.DownloadAdderStart>(UserProgramAddressSize))
//	return NRC_0x31_ROOR;
	//if(gDownloadService.DownloadAdderSize>UserProgramAddressSize)
	//return NRC_0x31_ROOR;
	//FLASH_UnlockBank1();
	//printf("\r\nUnlock");
	Para[0]=0x10;
	Para[1]=DownloadmaxNumberOfBlockLength;
	 gDownloadService.Type=RequestDownloadservice;
	gDownloadService.DownloadorUploadAdder=gDownloadService.DownloadAdderStart;
	gDownloadService.BlockNo=0;
	SetCFrameBTmin(5);//设置CF传输间隔
	memset(gDownloadService.DownloadBuff,0,256);
	gDownloadService.DownloadbufVail=0;
		memset(gDownloadService.DownloadBuff,0,4096);

	return 0;
}

/***************************
Prog_RequestUploadserviceservice
	  请求上传服务
****************************/
unsigned char Prog_RequestUploadserviceservice(unsigned char *buff,unsigned char len,unsigned char *Para)
{
//	unsigned char i;
//	if(len<3)
//			{
//				return NRC_0x13_IMLOIF;
//			}
//	if((buff[0]&0xf)+(buff[0]>>4)!=len-1)
//	return NRC_0x31_ROOR;
//	gDownloadService.DownloadAdderStart=0;
//	gDownloadService.DownloadAdderSize=0;
//	for(i=0;i<(buff[0]>>4);i++)
//	{
//		gDownloadService.DownloadAdderStart=gDownloadService.DownloadAdderStart<<8;
//		gDownloadService.DownloadAdderStart+=buff[i+1];
//	}
//	for(;i<(buff[0]&0xf)+(buff[0]>>4);i++)
//	{
//		gDownloadService.DownloadAdderSize=gDownloadService.DownloadAdderSize<<8;
//		gDownloadService.DownloadAdderSize+=buff[i+1];
//	}
//	printf("\r\nUploadAdderStart=%x UploadAdderSize=%d",gDownloadService.DownloadAdderStart,gDownloadService.DownloadAdderSize);

//	Para[0]=0x10;
//	Para[1]=DownloadmaxNumberOfBlockLength;
//	 gDownloadService.Type=RequestUploadservice;
//	gDownloadService.DownloadorUploadAdder=gDownloadService.DownloadAdderStart;
//	gDownloadService.BlockNo=0;
//	SetCFrameBTmin(5);//设置CF传输间隔
	return 0;
}
/***************************
Prog_RequestTransferExitservice
	 请求数据传输停止服务
****************************/
unsigned char Prog_RequestTransferExitservice(unsigned char *buff,unsigned char len)
{
//		u32 ret;
//	if( gDownloadService.Type==RequestDownloadservice)
//	{
//		//	if(gDownloadService.DownloadbufVail!=0&&gDownloadService.DownloadorUploadAdder>ConfigFileParameterAddressEntry)
//	{
//	//printf("\r\neDownloadAdderStart=%x %d",(u32)gDownloadService.DownloadorUploadAdder,gDownloadService.DownloadbufVail);
//	
////		ret=sectorPrepare(APP_START_SECTOR, APP_END_SECTOR);
////		if(ret)
////		{
////			printf("\r\nsectorPrepare|ramCopy0=%x",ret);
////			return NRC_0x72_GPF;//编程错误
////		}
//		//ret=WriteSystemDataToFlash((unsigned long)gDownloadService.DownloadorUploadAdder, gDownloadService.DownloadBuff, 4096);
//		//memset(gDownloadService.DownloadBuff,0,4096);
//		//if(ret==false)
//		{
//		//	printf("\r\nWriteSystemDataToFlash error");
//		//	return NRC_0x72_GPF;//编程错误
//		}
//	}
//	}
	SetCFrameBTmin(100);
	gDownloadService.Type=0;
	return 0;
}
/*********************************************************************************************************
** Function name:       Prog_TransferDataservice
** Descriptions:        数据传输服务
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
unsigned char Prog_TransferDataservice(unsigned char *buf,unsigned char len)
{
	
	if( gDownloadService.Type==RequestDownloadservice)
	{
//		u8 i;
		u32 ret;
//	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
//if(gDownloadService.DownloadorUploadAdder>(UserProgramAddressSize))
//	return NRC_0x31_ROOR;//地址范围错误
//	if(len-1!=DownloadmaxNumberOfBlockLength)
		//if(len-1!=16)
//	return NRC_0x31_ROOR;
	if(gDownloadService.DownloadorUploadAdder>gDownloadService.DownloadAdderStart+gDownloadService.DownloadAdderSize+DownloadmaxNumberOfBlockLength)
	{
		printf("\r\ngDownloadService.DownloadorUploadAdder=%x",gDownloadService.DownloadorUploadAdder);
			printf("\r\ngDownloadService.DownloadAdderStart=%x",gDownloadService.DownloadAdderStart);
			printf("\r\ngDownloadService.DownloadAdderSize=%x",gDownloadService.DownloadAdderSize);
		return NRC_0x24_RSE;//超出请求地址大小
	}
	
	if(gDownloadService.BlockNo!=buf[0])
		return NRC_0x73_WBSC;//块计数错误
	/********************************************************/
	//memcpy(gDownloadService.DownloadBuff+gDownloadService.DownloadbufVail,buf+1,DownloadmaxNumberOfBlockLength);
	//gDownloadService.DownloadbufVail+=DownloadmaxNumberOfBlockLength;
	//if(gDownloadService.DownloadbufVail==4096)
	{
		printf("\r\nDownAdder=%x ",(u32)gDownloadService.DownloadorUploadAdder);
		
		if((len-1)>DownloadmaxNumberOfBlockLength)
			return NRC_0x31_ROOR;
		else
		{
			u8 buffer[DownloadmaxNumberOfBlockLength];
			memcpy(buffer,buf+1,len-1);
				ret=WriteSystemDataToFlash(gDownloadService.DownloadorUploadAdder, buf+1,DownloadmaxNumberOfBlockLength);
		}
	
		//memset(gDownloadService.DownloadBuff,0,4096);
		//ret=sectorPrepare(APP_START_SECTOR, APP_END_SECTOR);
		if(ret==false)
		{
			printf("\r\nWriteSystemDataToFlash error");
			return NRC_0x72_GPF;//编程错误
		}
	//	ret=ramCopy(gDownloadService.DownloadorUploadAdder, (u32)gDownloadService.DownloadBuff, 4096);
		if(ret)
		{
		//	printf("\r\nsectorPrepare|ramCopy3=%x",ret);
			//return NRC_0x72_GPF;//编程错误
		}

		gDownloadService.DownloadorUploadAdder+=DownloadmaxNumberOfBlockLength;
		gDownloadService.DownloadbufVail=0;
	}
		gDownloadService.BlockNo++;
	return 0;
	}
//	else if( gDownloadService.Type==RequestUploadservice)
//	{
//		printf("\r\n read Address=%x :",(unsigned int)gDownloadService.DownloadorUploadAdder);
////		if(gDownloadService.DownloadorUploadAdder>(UserProgramAddressSize))
////			return NRC_0x31_ROOR;
//	if(gDownloadService.DownloadorUploadAdder>gDownloadService.DownloadAdderStart+gDownloadService.DownloadAdderSize)
//		return NRC_0x24_RSE;
//	/********************************************************/
//		if(gDownloadService.BlockNo!=buf[0])
//		return NRC_0x73_WBSC;
//		{
//			u8 i;
//			for(i=0;i<32;i++)
//			{
//				printf(" %02x",*( uint32_t*)(gDownloadService.DownloadorUploadAdder+i)&0xff);
//			}
//		}
//		gDownloadService.DownloadorUploadAdder+=32;
//		gDownloadService.BlockNo++;
//		return 0;
//		
//	}
	else{
		printf("\r\ngDownloadService.Type=%x",gDownloadService.Type);
			return NRC_0x24_RSE;
	}
}
