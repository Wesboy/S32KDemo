#ifndef __CONFIG_H__
#define __CONFIG_H__

//Flash Memory Configure

//0x0000_0000-0x0001_FFFF 512KB
#define Flash_Entry 0x00000000
#define Flash_Size  0x80000

//0x0000_0000-0x0000_3FFF  16KB             // 用于存放bootloader的程序代码
#define BootloaderProgramFlashAddressEntry  	     Flash_Entry
#define BootloaderProgramFlashSize                   0x6000


//0x0000_5000-0x0000_51FF 512B              // 存放一些数据结构体，如编译时间等
#define ParameterSection1FlashEntry                  (BootloaderProgramFlashAddressEntry+BootloaderProgramFlashSize)   
#define ParameterSection1FlashSize                   0x1000

//0x0000_5200-0x0000_5FFF 512B            // 暂时没有使用,备用
#define ParameterSection2FlashEntry                  (ParameterSection1FlashEntry+ParameterSection1FlashSize)
#define ParameterSection2FlashSize                   0x200

//0x0000_6000-0x0007_FFFF 488K              // 用于存放System的程序代码
#define SystemProgramAddressFlashEntry                 (ParameterSection2FlashEntry+ParameterSection2FlashSize)
#define SystemProgramAddressFlashSize                  (Flash_Size - BootloaderProgramFlashSize  - ParameterSection1FlashSize - ParameterSection2FlashSize)

//Ram Memory Configure
//0x1FFF_8000-0x1FFF_FFFF 32KB
#define Ram1_Entry 0x1FFF8000
#define Ram1_Size  0x8000

//0x2000_0000-0x2000_6FFF 28KB
#define Ram2_Entry 0x20000000
#define Ram2_Size  0x7000


/************ Parameter Section RAM ************/
//0x2000_0000-0x2000_03FF 1KB           // 存放RDS Pi Af列表的数据 (RadioRdsPiAfListAddress)
#define ParameterSection0Ram1Entry     (Ram2_Entry)
#define ParameterSection0Ram1Size      0x400

//0x2000_0400-0x2000_05FF 512B          // 用于System程序保存非车型相关的变量 (paramInfo)
#define ParameterSection1Ram1Entry     (ParameterSection0Ram1Entry + ParameterSection0Ram1Size)
#define ParameterSection1Ram1Size      0x200

//0x2000_0600-0x2000_07FF 512B          // 用于System程序保存车型相关的变量
#define ParameterSection2Ram1Entry     (ParameterSection1Ram1Entry + ParameterSection1Ram1Size)
#define ParameterSection2Ram1Size      0x200

//0x2000_0800-0x2000_081F 32B           // 用于程序间共享的变量(该空间为其他处理器提供空间代替LPC_RTC->GPREGx寄存器)
#define ParameterSection3Ram1Entry     (ParameterSection2Ram1Entry+ParameterSection2Ram1Size)
#define ParameterSection3Ram1Size      0x20


//0x2000_0000-0x2000_081F 2K32B         // 所有ParameterSectionRam1Size占有的空间
#define ALLParameterSectionRam1Entry    (ParameterSection0Ram1Entry)
#define ALLParameterSectionRam1Size     (ParameterSection0Ram1Size + ParameterSection1Ram1Size + ParameterSection2Ram1Size + ParameterSection3Ram1Size)


/************ update RAM ************/
//0x1FFF_8000-0x1FFF_FFFF 32KB          // 用来作为update程序的运行内存
#define UpdaterRamRam1Entry            Ram1_Entry
#define UpdaterRamRam1Size             Ram1_Size

//0x2000_0820-0x2000_481F 16KB          // 用来作为update程序的存放
#define UpdaterRomRam2Entry            (ALLParameterSectionRam1Entry + ALLParameterSectionRam1Size)
#define UpdaterRomRam2Size             (0x4000) // 减去Parameter Section Size

//0x2000_4820-0x2000_6FFF  9K992B       // 用来作为update程序的运行内存
#define UpdaterRamRam2Entry            (UpdaterRomRam2Entry + UpdaterRomRam2Size)
#define UpdaterRamRam2Size             (Ram2_Size - UpdaterRomRam2Size - ALLParameterSectionRam1Size)


/************ bootloader RAM ************/
//0x1FFF_8000-0x1FFF_FFFF 32KB          // 用来作为bootloader运行的内存
#define BootloaderRam1Entry            Ram1_Entry
#define BootloaderRam1Size             Ram1_Size

//0x2000_4820-0x2000_6FFF  9K992B       // 用来作为bootloader运行的内存
#define BootloaderRam2Entry            UpdaterRamRam2Entry
#define BootloaderRam2Size             UpdaterRamRam2Size


/************ bootloaderEncrypt RAM ************/
//0x1FFF_8000-0x1FFF_FFFF 32KB          // 用来作为bootloader_encrypt运行的内存
#define BootloaderEncryptRam1Entry     Ram1_Entry
#define BootloaderEncryptRam1Size      Ram1_Size

//0x2000_4820-0x2000_6FFF  9K992B       // 用来作为bootloader_encrypt运行的内存
#define BootloaderEncryptRam2Entry     UpdaterRamRam2Entry
#define BootloaderEncryptRam2Size      UpdaterRamRam2Size


/************ System RAM ************/
//0x1FFF_8000-0x1FFF_FFFF 32KB          // 用来作为system程序的运行内存
#define SystemProgramRam1Entry            Ram1_Entry
#define SystemProgramRam1Size             Ram1_Size

//0x2000_4820-0x2000_6FFF  25K992       // 用来作为system程序的运行内存
#define SystemProgramRam2Entry            (ALLParameterSectionRam1Entry + ALLParameterSectionRam1Size)
#define SystemProgramRam2Size             (Ram2_Size  - ALLParameterSectionRam1Size) // 减去Parameter Section Size

//all 

#define BootloaderProgramAddressEntry               BootloaderProgramFlashAddressEntry
#define BootloaderProgramSize                       BootloaderProgramFlashSize




#define ParameterSection1ConfigAddressEntry			ParameterSection1FlashEntry
#define ParameterSection1ConfigSize			        ParameterSection1FlashSize

#define ParameterSection2ConfigAddressEntry			ParameterSection2FlashEntry
#define ParameterSection2ConfigSize			        ParameterSection2FlashSize

#define UpdaterProgramAddressEntry                  UpdaterRomRam2Entry
#define UpdaterProgramSize                          UpdaterRomRam2Size

#define SystemProgramAddressEntry			        SystemProgramAddressFlashEntry
#define SystemProgramSize			                SystemProgramAddressFlashSize


#define FLASH_SECTOR_SIZE       FEATURE_FLS_PF_BLOCK_SECTOR_SIZE


//用于在SystemProgram区域中指定一个地址,使用一个字节的大小存放一个数据,用于表示当前是否有第一次上电运行程序
#define BirthmarkAddress                            (SystemProgramAddressEntry+0x34)

// RAM space for radio rds af 0x2000_6A00 ~ 0x2000_6DFF	(size is 0x400)-1KB
#define RadioRdsPiAfListAddress			ParameterSection0Ram1Entry

#endif

