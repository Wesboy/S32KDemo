
#include "Interface.h"


flash_ssd_config_t flashConfig;


void FlashProgramInit(void)
{
    uint32_t bRes = 0;
    
    const flash_user_config_t Flash_InitConfig = {
        .PFlashBase  = 0x00000000U,
        .PFlashSize  = 0x00080000U,
        .DFlashBase  = 0x10000000U,
        .EERAMBase   = 0x14000000U,
        .CallBack    = NULL_CALLBACK
    };

    INT_SYS_DisableIRQ(FTFC_IRQn);

    bRes = FLASH_DRV_Init(&Flash_InitConfig, &flashConfig);

    if(bRes == STATUS_SUCCESS)
    {
        printf("\r\nFLASH_DRV_Init() SUCCESS ");
    }
    else 
    {
        bRes = FLASH_DRV_Init(&Flash_InitConfig, &flashConfig);
        
			printf("\r\n RE FLASH_DRV_Init() %d ",!bRes);
    }
}

//写入8的倍数个字节
static bool FlashProgram(uint32_t u32NVMTargetAddress, uint8_t *pData, uint16_t u16SizeBytes)
{
	bool bRes = true;
    status_t ret;
    _DI();
	ret=FLASH_DRV_Program(&flashConfig, u32NVMTargetAddress , u16SizeBytes, pData);

    if( STATUS_SUCCESS != ret )
    {
			
        printf("\r\n FLASH_DRV_Program() false");
        return false;
    }
    _EI();

	return bRes;
}


bool WriteSystemDataToFlash(uint32_t address,uint8_t *pdata,uint16_t len)
{
    bool bRes = false;
	
    if ( 0 != len )
    {
		bRes = FlashProgram(address,pdata,len);     
    }
	
	return bRes;
}

//擦除用户程序占用的flash空间 （ 包括 ParameterSection1 和 ParameterSection2 ）
bool EraseSystemFlashSpace(void)
{
	uint16_t sector_count = 0;
	uint16_t sector_surplus = 0;
    
	sector_count = (Flash_Size - BootloaderProgramSize  )/FLASH_SECTOR_SIZE;
    sector_surplus = (Flash_Size - BootloaderProgramSize  )%FLASH_SECTOR_SIZE;
    
    _DI();
    if( STATUS_SUCCESS != FLASH_DRV_EraseSector(&flashConfig, ParameterSection1ConfigAddressEntry , sector_count * FLASH_SECTOR_SIZE) )
    {
        printf("\r\n FLASH_DRV_EraseSector1() false");
        return false;
    }
    if(sector_surplus > 0)
    {
        if( STATUS_SUCCESS != FLASH_DRV_EraseSector(&flashConfig, Flash_Size - FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE) )
        {
            printf("\r\n FLASH_DRV_EraseSector2() false");
            return false;
        }
    }
    _EI();
    
	return true ;
}

////擦除配置文件占用的flash空间
//bool EraseConfigFileFlashSpace(void)
//{
//	bool bRes = true;
//	
//	uint16_t sector_count = 0;
//	uint16_t sector_surplus = 0;
//    
//    if(ConfigFileParameterSize<FLASH_SECTOR_SIZE)
//        return false;
//	sector_count = (ConfigFileParameterSize )/FLASH_SECTOR_SIZE;
//    sector_surplus = (ConfigFileParameterSize )%FLASH_SECTOR_SIZE;
//    _DI();
//    if( STATUS_SUCCESS != FLASH_DRV_EraseSector(&flashConfig, ConfigFileParameterAddressEntry , sector_count * FLASH_SECTOR_SIZE) )
//    {
//        printf("\r\n FLASH_DRV_EraseSector1() false");
//        return false;
//    }
//    if(sector_surplus > 0)
//    {
//        if( STATUS_SUCCESS != FLASH_DRV_EraseSector(&flashConfig, ConfigFileParameterAddressEntry +  ConfigFileParameterSize - FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE) )
//        {
//            printf("\r\n FLASH_DRV_EraseSector2() false");
//            return false;
//        }
//    }
//    _EI();
//	
//	return bRes ;
//}


//bool WriteSystemVersionToFlash(void)
//{
//	bool bRes = true;
//    
//    printf("\r\n SystemGuideConfig size:%d",sizeof(SystemGuideConfig));

//	if( 0 == FlashProgram(ParameterSection1ConfigAddressEntry,(uint8_t *)&SystemGuideConfig, sizeof( SystemGuideConfig)) )
//	{
//		bRes = false;
//	}	

//	return bRes;
//}

//bool WriteConfigFileDataToflash(uint8_t *pdata,uint16_t len)
//{
//	bool bRes = true;
//    
//    printf("\r\n ConfigFile len:%d",sizeof(SystemGuideConfig));
//	
//	if( 0 == FlashProgram( ConfigFileParameterAddressEntry, pdata, len))
//	{
//		bRes = false;
//	}	
//	
//	return bRes;
//}

