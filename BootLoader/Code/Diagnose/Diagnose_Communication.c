/* Diagnose_Communication.c                              Flyaudio 2019
 * 描述: 诊断的通讯协议
 *              
 * 2019 02 20 移植预研阶段的诊断升级模块
 * 
 * 
 */
#include "Diagnose_Communication.h"
#include "string.h"
#include "stdio.h"
#define N_USdata_Len 255   //ISO15765最大长度为 0xFFFFFFFF
#define defN_USdata_BS  0
#define defN_USdata_STmin 100
enum
{ 
	SingleFrame=0,
	FirstFrame,
	ConsecutiveFrame,
	FlowControl,
	WaitFlowControl,
	NormalFrame,
	N_PDU_SF=0,
	N_PDU_FF,
	N_PDU_CF,
	N_PDU_FC,
	ConsecutiveFrame_CTS=0,
	ConsecutiveFrame_WT,
	ConsecutiveFrame_OVFLW,	
};

typedef struct {
	u8 data[N_USdata_Len];
	u8 Status;
	u8 RT_Number;
	U8 ConsecutiveFrame_Status;
	U32 ConsecutiveFrame_STmin;
	U32 ConsecutiveFrame_timer;
	u32 len;
	u8 FlowControl_BS;
	U8 FlowControl_RTBS;
	u32 Timer_BS;
}tN_USdata;
tN_USdata N_USdataSend;
tN_USdata N_USdataRec;
unsigned  int gCFrameBTmin=defN_USdata_STmin;
void SetCFrameBTmin(const unsigned  int val)
{
	gCFrameBTmin=val;
}
static void FlowControl_Send(u8 status) 
{
	u8 buf[8]={0x30,0,0,0,0,0,0,0};
	
	switch(status)
	{
		case ConsecutiveFrame_CTS:
			buf[0]=0x30;
			break;
		case ConsecutiveFrame_WT:
			buf[0]=0x31;
			break;
		case ConsecutiveFrame_OVFLW:
			buf[0]=0x32;
			break;
		default:
			return;
			
	}
	buf[1]=defN_USdata_BS;
	buf[2]=gCFrameBTmin;

	Diagnose_Communication_CAN_Send(buf,8);
}
static void SingleFrame_Send(u8 *data,u32 len)
{
	u8 buf[8]={0,0,0,0,0,0,0,0};
	if(len>7)
		return;
	buf[0]=len;
	memcpy(buf+1,data,len);

	Diagnose_Communication_CAN_Send(buf,len);

	N_USdataSend.Status=NormalFrame;
}
static void FirstFrame_Send(u8 *data,u32 len)
{
	u8 buf[8]={0x10,0,0,0,0,0,0,0};
	buf[0]|=len>>8;
	buf[1]=len;
	memcpy(buf+2,data,6);
	Diagnose_Communication_CAN_Send(buf,8);
	N_USdataSend.Status=FirstFrame;
}
static void ConsecutiveFrame_Send(u8 SN,u8 *data,u32 len)
{
	u8 buf[8]={0x20,0,0,0,0,0,0,0};
	if(len>7)
		return ;
	buf[0]|=SN&0xf;

	memcpy(buf+1,data,len);
	Diagnose_Communication_CAN_Send(buf,8);

}
static void N_USdata_SendPro()
{
	switch(N_USdataSend.Status)
	{
		case NormalFrame:
			break;
		case FirstFrame:
			N_USdataSend.ConsecutiveFrame_Status=ConsecutiveFrame_WT;
			N_USdataSend.Status=WaitFlowControl;
			ResetUserTimer(&N_USdataSend.Timer_BS);
			break;
		case ConsecutiveFrame:
		
			if(ReadUserTimer(&N_USdataSend.ConsecutiveFrame_timer)>N_USdataSend.ConsecutiveFrame_STmin)
			{
//	printf("\r\nConsecutiveFrameConsecutiveFrame %x %x",ReadUserTimer(&N_USdataSend.ConsecutiveFrame_timer),N_USdataSend.ConsecutiveFrame_STmin);
				
				ResetUserTimer(&N_USdataSend.ConsecutiveFrame_timer);
			  if((N_USdataSend.RT_Number*7+6)>=N_USdataSend.len)
				{
					ConsecutiveFrame_Send(N_USdataSend.RT_Number,N_USdataSend.data+ N_USdataSend.RT_Number*7-1,\
				(N_USdataSend.len-N_USdataSend.RT_Number*7+1));	
					N_USdataSend.Status=NormalFrame;
				}
				else
				{
					ConsecutiveFrame_Send(N_USdataSend.RT_Number,N_USdataSend.data+ N_USdataSend.RT_Number*7-1,7);	
				}
				N_USdataSend.RT_Number++;
				if(((++N_USdataSend.FlowControl_RTBS)>=N_USdataSend.FlowControl_BS)&&N_USdataSend.FlowControl_BS!=0)
				{
					N_USdataSend.Status=WaitFlowControl;
					N_USdataSend.ConsecutiveFrame_Status=ConsecutiveFrame_WT;
					ResetUserTimer(&N_USdataSend.Timer_BS);
				}
			}
			break;
		case WaitFlowControl:
			if(N_USdataSend.ConsecutiveFrame_Status==ConsecutiveFrame_CTS)
			{
			
				N_USdataSend.FlowControl_RTBS=0;
				N_USdataSend.Status=ConsecutiveFrame;
				return;
			}
			if(N_USdataSend.ConsecutiveFrame_Status==ConsecutiveFrame_OVFLW)
			{
			
				N_USdataSend.Status=NormalFrame;
				return;
			}
			if(ReadUserTimer(&N_USdataSend.Timer_BS)>1000)
			{
				printf("\r\n##WaitFlowControl outtime");
				N_USdataSend.Status=NormalFrame;
			}
				
			break;

	}
}
/****************************************
***	名称：	Diagnose_Communication_Init
***	描述： 
*** 备注：诊断通讯的初始化 can
****************************************/
void Diagnose_Communication_Init()
{
	memset(&N_USdataSend,0,sizeof(tN_USdata));
	memset(&N_USdataRec,0,sizeof(tN_USdata));
	N_USdataRec.Status=NormalFrame;
	N_USdataSend.Status=NormalFrame;
	N_USdataRec.FlowControl_BS=defN_USdata_BS;
	N_USdataRec.ConsecutiveFrame_STmin=defN_USdata_STmin;
	N_USdataSend.FlowControl_BS=defN_USdata_BS;
	N_USdataSend.ConsecutiveFrame_STmin=defN_USdata_STmin;
	
}
/****************************************
***	名称：	Diagnose_Communication_Request
***	描述：  Service.C 使用此函数发送服务内容
*** 备注：  
****************************************/
void Diagnose_Communication_Request(u8 *data,u32 len)
{
	if(len>N_USdata_Len)//ISO15765最大长度为 0xFFFFFFFF
		return ;

	if(len<8)
	{
	
		SingleFrame_Send(data,len);//单帧
		return ;
	}

	FirstFrame_Send(data,len);//多针首
	N_USdataSend.len=len;
	N_USdataSend.Status=FirstFrame;
	N_USdataSend.RT_Number=1;
	ResetUserTimer(&N_USdataSend.ConsecutiveFrame_timer);
	memcpy(N_USdataSend.data,data,len);
	
//	memcpy(&N_USdata.data,data,len);
	
}
/****************************************
***	名称：	Diagnose_Communication_CAN_Receive
***	描述：  单帧CAN数据入口的接口
*** 备注：  
****************************************/
  void Diagnose_Communication_CAN_Receive(u8 *data,u8 len)
{
	u8 N_PDU_Type=0;
	u32 N_PDU_Len=0;
	if(len==0||len>8)
		return;
	N_PDU_Type=data[0]>>4;
//	printf("\r\nDiagnose_Communication_CAN_Receive N_PDU_Type=%x",N_PDU_Type);
	switch(N_PDU_Type)
	{
		case N_PDU_SF:
				N_PDU_Len =data[0]&0xf;
			if(N_PDU_Len==0||N_PDU_Len>7)
				return;
			if(N_PDU_Len>len-1)
				return;
			Diagnose_Communication_Recive(data+1,N_PDU_Len);
			break;
		case N_PDU_FF:
			N_PDU_Len = ((data[0]&0xf)<<8)|data[1];
			if(N_PDU_Len<8||N_PDU_Len>N_USdata_Len||len!=8)
				return;
			memcpy(N_USdataRec.data,data+2,6);
			N_USdataRec.Status=N_PDU_FF;
			N_USdataRec.RT_Number=0;
			N_USdataRec.FlowControl_RTBS=0;
			N_USdataRec.len=N_PDU_Len;
			FlowControl_Send(ConsecutiveFrame_CTS);
			break;
		case N_PDU_CF:
			if(len!=8)
				return;
			
			if(N_USdataRec.Status==N_PDU_FF||N_USdataRec.Status==N_PDU_CF)
			{
			
				N_USdataRec.RT_Number++;
				N_USdataRec.FlowControl_RTBS++;
				if(N_USdataRec.RT_Number%16!=(data[0]&0xf))
				{
					printf("\r\n###N_USdataRec SN error");
					N_USdataRec.Status=NormalFrame;
					return;
				}
				if(N_USdataRec.FlowControl_RTBS%N_USdataRec.FlowControl_BS==0)
				FlowControl_Send(ConsecutiveFrame_CTS);
				memcpy(N_USdataRec.data-1+N_USdataRec.RT_Number*7,data+1,7);
				if((9+N_USdataRec.RT_Number*7)>=N_USdataRec.len)
				{
				
					Diagnose_Communication_Recive(N_USdataRec.data,N_USdataRec.len);
					N_USdataRec.Status=NormalFrame;
					return;
				}
				
				
				
			}
			N_USdataRec.Status=N_PDU_CF;
			break;
		case N_PDU_FC:
		
			if(len<3)
				return;
		
			if(N_USdataSend.Status==WaitFlowControl)
			{
			
				N_USdataSend.ConsecutiveFrame_Status=data[0]&0xf;
				N_USdataSend.FlowControl_BS=data[1];
				N_USdataSend.ConsecutiveFrame_STmin=data[2];
				if(N_USdataSend.ConsecutiveFrame_STmin>127)
				{
					
					N_USdataSend.ConsecutiveFrame_STmin=127;
				}
					
			}
			break;
	}
	
	
	
	
	
}
/****************************************
***	名称：	Diagnose_Communication_Pro
***	描述：  诊断通讯的处理过程
*** 备注：  需要轮询调用
****************************************/
void Diagnose_Communication_Pro()
{
	N_USdata_SendPro();
}



//void Diagnose_Communication_CAN_Send(u8 *data,u8 len)
//{
//	u8 i;
//		printf("\r\nSed CAN:");
//	for(i=0;i<len;i++)
//		printf(" %02x",data[i]);
//	for(;i<8;i++)
//		printf(" 00");
//}
//void Diagnose_Communication_CAN_Receive(u8 *data,u8 len)
//{
//	u8 i;
//		printf("\r\nRec CAN:");
//	for(i=0;i<len;i++)
//		printf(" %02x",data[i]);
//	for(;i<8;i++)
//		printf(" 00");
//	N_USdata_ReceiveDeal(data,len);
//	
//}







