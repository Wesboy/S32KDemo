

#include "Interface.h"


#define SysGuideCfgAddressEntry ParameterSection2FlashEntry

//其用来判断是否为烧写bootloader程序后第一次上电运行
const unsigned CRP_Key __attribute__((at(BirthmarkAddress))) = BIRTHMARKDATA; //未加密胎记

/**/
void Programming_SetHaveSystemFlag(void)
{
//		CRP_VERSION1=0x12345678;
//	CRP_VERSION2=0x87654321;
	u8 buf[]={0x78,0x56,0x34,0x12,0x21,0x43,0x65,0x87};
	printf("\r\nProgramming_SetHaveSystemFlag");
	WriteSystemDataToFlash(ParameterSection2FlashEntry,buf,8);
	
	
	
}
//返回当前flash中是否有system的程序,true表示有,false表示没有

static bool CheckWetherHaveSystemProgram(void)
{
    bool bRes = false;


   printf("\r\nParameterSection2FlashEntry=%x",(*(unsigned int*)ParameterSection2FlashEntry));
	printf("\r\nParameterSection2FlashEntry2=%x",(*(unsigned int*)(ParameterSection2FlashEntry+4)));
		if((0x12345678==(*(unsigned int*)ParameterSection2FlashEntry))&&(0x87654321==(*(unsigned int*)((ParameterSection2FlashEntry)+4))))
		{

			bRes = true;
		}
		
    return bRes;
}
//返回当前是否要升级system程序

static bool CheckWetherUpdateProgram(void)
{
    bool bRes = false;
   
	
	
    if ( USER_WANT2UPDATE_PARA == Bootloader_GetUpdataSystemFlag() )
    {
        bRes = true;
    }
    
    return bRes;
}


//返回当前是否为第一次上电运行程序
//原来切换到bootloader_encrypt程序中才执行擦除操作,现在在bootloader中完成,暂时不再需要bootloader_encrypt程序
static void CheckWetherFirstPowerOn(void)
{
   if ( (*(unsigned int*)ParameterSection2FlashEntry) == 0X77777777 &&(*(unsigned int*)(ParameterSection2FlashEntry+0X4))==0X33333333 )
    {

        if ( EraseSystemFlashSpace() )
        {
					printf("\r\nEarase FlashSpace successed.");
           printf("\r\n Switch to BootloaderProgramAddressEntry.");  

        }

    }
}
void CheckPermission(void)
{
    bool bHaveSystemProgram = false;
	  bool bUpdateSystemProgram = false;
    bHaveSystemProgram = CheckWetherHaveSystemProgram();
	  CheckWetherFirstPowerOn();
    bUpdateSystemProgram = CheckWetherUpdateProgram();
	 printf("\r\nbHaveSystemProgram:%d,bUpdateSystemProgram:%d",bHaveSystemProgram,bUpdateSystemProgram);
    if ( bHaveSystemProgram &&!bUpdateSystemProgram)
    {
        printf("\r\n boot into system.");
        GoT0WhatStatus(SystemProgramAddressEntry);
    } 
}


























