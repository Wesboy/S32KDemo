/*********************************************Copyright (c)***********************************************
**                                Guangzhou ZLG MCU Technology Co., Ltd.
**
**                                        http://www.zlgmcu.com
**
**      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
**  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
**  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
**  承担任何责任。
**                                                                        ——广州周立功单片机科技有限公司
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           IAP.c
** Last modified Date:  2013-05-20
** Last Version:        V1.0
** Descriptions:        IAP升级-IAP函数
**
**--------------------------------------------------------------------------------------------------------
** Created by:          ChenQiliang
** Created date:        2013-05-24
** Version:             V1.0
** Descriptions:        添加用户应用程序
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
** Rechecked by:        
*********************************************************************************************************/
#include "IAP.h"

/*
 *  定义函数指针  
 */
void (*IAP_Entry)(INT32U *param_tab, INT32U *result_tab) = (void(*)())IAP_ENTER_ADR;

INT32U  paramin[5];                                                     /* IAP入口参数缓冲区            */
INT32U  paramout[4];                                                    /* IAP出口参数缓冲区            */

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP操作扇区选择，命令代码50
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  sectorPrepare (INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_Prepare;                                           /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;                            
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */
   
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        复制RAM的数据到FLASH，命令代码51
** input parameters:    dst:            目标地址，即FLASH起始地址。以512字节为分界
**                      src:            源地址，即RAM地址。地址必须字对齐
**                      no:             复制字节个数，为512/1024/4096/8192
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  ramCopy(INT32U dst, INT32U src, INT32U no)
{  
    paramin[0] = IAP_RAMTOFLASH;                                        /* 设置命令字                   */
    paramin[1] = dst;                                                   /* 设置参数                     */
    paramin[2] = src;
    paramin[3] = no;
    paramin[4] = IAP_FCCLK;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */
    
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        扇区擦除，命令代码52
** input parameters:    sec1            起始扇区
**                      sec2            终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  sectorErase (INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_ERASESECTOR;                                       /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;
    paramin[3] = IAP_FCCLK;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */
   
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        扇区查空，命令代码53
** input parameters:    sec1:           起始扇区
**                      sec2:           终止扇区92
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  blankChk (INT8U sec1, INT8U sec2)
{  
    paramin[0] = IAP_BLANKCHK;                                          /* 设置命令字                   */
    paramin[1] = sec1;                                                  /* 设置参数                     */
    paramin[2] = sec2;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        读器件ID，命令代码54
** input parameters:    无
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  parIdRead (void)
{  
    paramin[0] = IAP_READPARTID;                                        /* 设置命令字                   */
    
    paramin[1] = 0x00;
    paramin[2] = 0x00;
    paramin[3] = 0x00;
    paramin[4] = 0x00;
    
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    
    
    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        读Boot版本号，命令代码55
** input parameters:    无
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  codeIdBoot (void)
{  
    paramin[0] = IAP_BOOTCODEID;                                        /* 设置命令字                   */
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        校验数据，命令代码56
** input parameters:    dst:            目标地址，即RAM/FLASH起始地址。地址必须字对齐
**                      src:            源地址，即FLASH/RAM地址。地址必须字对齐
**                      no:             复制字节个数，必须能被4整除
** output parameters:   paramout[0]:    IAP操作状态码,IAP返回值
** Returned value:      paramout[0]:    IAP操作状态码,IAP返回值
*********************************************************************************************************/
INT32U  dataCompare (INT32U dst, INT32U src, INT32U no)
{  
    paramin[0] = IAP_COMPARE;                                           /* 设置命令字                   */
    paramin[1] = dst;                                                   /* 设置参数                     */
    paramin[2] = src;
    paramin[3] = no;
    (*IAP_Entry)(paramin, paramout);                                    /* 调用IAP服务程序              */

    return (paramout[0]);                                               /* 返回状态码                   */
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/

