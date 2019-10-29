/* FlyPDUNetwork.c                  Flyaudio 2019
 * 描述: 诊断的通讯协议
 *              
 * 2019 02 20 移植预研阶段的诊断升级模块
 * 2019 06 27 整理为pdu路由处理模块，
 * 			  主要功能：1、对发送和接收数据信息
 * 					   2、报告发送和接收成功	
 * 	
 * 			  输出：数据信息和传输结果信息 data and result
 */
#include "FlyPDUNetwork.h"
#include "interface.h"
#include "Servce.h"
#include "systick.h"

static N_USdata N_USdataSend;
//static N_USdata N_USdataRec;
static unsigned int gCFrameBTmin = defN_USdata_STmin;

static struct N_time m_TimePeriod;
static CommuParam TxParam;
static CommuParam RxParam;
static N_RESULT_INFO N_Ret;
static eNWL_Status gNetworkStatus = NWL_IDLE;
static eTranStep gTxStep;
static eRecvStep gRxStep;
static uint8_t RxBuff[7];
static uint8_t NetworkPduRecvBuf[N_USdata_Len];
static N_PDU RxPDU;
static N_PDU TxPDU;
static eAddrFormat gAddrType;
static uint8_t *pCFData;

void SetCFBTmin(const unsigned int val)
{
	gCFrameBTmin = val;
}

/****************************************
***	名称：	FlyPDUNetwork_RecvFromCAN
***	描述：  对外接口接收CAN总线数据
*** 备注：  
****************************************/

void FlyPDUNetwork_RecvFromCAN(u32 ID, u8 *buf, u8 len)
{
	RxPDU.ID = ID;
	RxPDU.N_SA = (uint8_t)(ID >> 8);
	RxPDU.N_TA = (uint8_t)ID;
	RxPDU.N_PciType = (ePCIType)(buf[0] >> 4);
	if (ID == CAN_ID_DIAGNOSIS_FUNCTION || ((ID & 0x0000FF00) == 0x0000FF00))
	{
		RxPDU.N_TAtype = type_fun;
	}
	else
	{
		RxPDU.N_TAtype = type_phy;
	}

	memcpy(RxPDU.data, buf, len);

	FlyPDUNetwork_RecvBufFromLinkLayer(RxPDU.N_PciType);
}

void NetworkLayer_TxEnd(void)
{
	gTxStep = TX_IDLE;
	gNetworkStatus = NWL_IDLE;
}

void NetworkLayer_TxStart(void)
{
	gTxStep = TX_WAIT_FF_CONF;
	gNetworkStatus = NWL_TRANSMITTING;
}

void NetworkLayer_RxEnd(void)
{
	//printk("RX end\r\n");
	gRxStep = RX_IDLE;
	gNetworkStatus = NWL_IDLE;
	RxParam.TotalDataNumber = 0;
	RxParam.CompletedDataNumber = 0;
	RxParam.CompletedNumberInBlock = 0;
}

void NetworkLayer_RxStart(void)
{
	gRxStep = RX_WAIT_FC_REQ;
	gNetworkStatus = NWL_RECIVING;
	RxParam.CompletedDataNumber = 0;
	RxParam.CompletedNumberInBlock = 0;
}

/****************************************
***	名称：	FlyNetwork_RecvSF
***	描述：  对单帧数据进行处理
*** 备注：  
****************************************/
static void FlyNetwork_RecvSF(void)
{
	RxPDU.PDU_L = RxPDU.data[0] & 0xf;
	if (RxPDU.PDU_L < 1 || RxPDU.PDU_L > 7)
		return;
	if (RxPDU.PDU_L > RxPDU.DLC - 1)
		return;

	if (gNetworkStatus == NWL_RECIVING)
	{
		N_Ret = N_UNEXP_PDU;
		gNetworkStatus = NWL_IDLE;
	}
	else if (gNetworkStatus == NWL_TRANSMITTING || gNetworkStatus == NWL_IDLE)
	{
		/* code */
		N_Ret = N_OK;
		gNetworkStatus = NWL_IDLE;
	}

	RxBuff[0] = RxPDU.data[1];
	RxBuff[1] = RxPDU.data[2];
	RxBuff[2] = RxPDU.data[3];
	RxBuff[3] = RxPDU.data[4];
	RxBuff[4] = RxPDU.data[5];
	RxBuff[5] = RxPDU.data[6];
	RxBuff[6] = RxPDU.data[7];
	N_USData_indication(SF, RxPDU.Mtype, RxPDU.N_SA, RxPDU.N_TA, RxPDU.N_TAtype, RxPDU.N_AE, RxBuff, RxPDU.PDU_L, N_Ret);
}

/****************************************
***	名称：	FlyNetwork_RecvFF
***	描述：  对首帧数据进行处理
*** 备注：  
****************************************/
static void FlyNetwork_RecvFF(void)
{
	RxPDU.PDU_L = ((RxPDU.data[0] & 0xf) << 8) | RxPDU.data[1];

	if (RxPDU.PDU_L < 0x07)
	{
		//ignore this frame
		//do nothing!
	}
	else if (RxPDU.PDU_L > N_USdata_Len)
	{
		//invalid and send flow control with flowStatus= overflow
		FlyPDUNetwork_FC_SendtoLink(pdu_CF_OVFLW);
	}
	else
	{
		if (RxPDU.DLC == 8)
		{
			RxBuff[0] = RxPDU.data[2];
			RxBuff[1] = RxPDU.data[3];
			RxBuff[2] = RxPDU.data[4];
			RxBuff[3] = RxPDU.data[5];
			RxBuff[4] = RxPDU.data[6];
			RxBuff[5] = RxPDU.data[7];

			if (gNetworkStatus == NWL_IDLE)
			{
				N_Ret = N_OK;
				gNetworkStatus = NWL_RECIVING;
			}
			else if (gNetworkStatus == NWL_RECIVING)
			{
				N_Ret = N_UNEXP_PDU;
				gNetworkStatus = NWL_RECIVING; //new start of reception
			}
			else if (gNetworkStatus == NWL_TRANSMITTING)
			{
				N_Ret = N_OK;
				gNetworkStatus = NWL_RECIVING; //only when Full-duplex
			}

			RxParam.SN = 0;
			RxParam.FS_Type = CTS;
			N_USData_FF_indication(RxPDU.Mtype, RxPDU.N_SA, RxPDU.N_TA, RxPDU.N_TAtype, RxPDU.N_AE, RxPDU.PDU_L);
			N_USData_indication(FF, RxPDU.Mtype, RxPDU.N_SA, RxPDU.N_TA, RxPDU.N_TAtype, RxPDU.N_AE, RxBuff, RxPDU.PDU_L, N_Ret);
		}
	}
}

/****************************************
***	名称：	FlyNetwork_RecvCF
***	描述：  对连续帧数据进行处理
*** 备注：  
****************************************/
static void FlyNetwork_RecvCF(void)
{
	if (gNetworkStatus == NWL_RECIVING)
	{
		uint8_t CurrFrameLength = 0;
		if (((RxParam.SN + 1) & 0x0F) == (RxPDU.data[0] & 0x0F))
		{
			N_Ret = N_OK;
			if (RxParam.CompletedDataNumber < RxParam.TotalDataNumber)
			{
				RxParam.SN = RxPDU.data[0];
				RxBuff[0] = RxPDU.data[1];
				RxBuff[1] = RxPDU.data[2];
				RxBuff[2] = RxPDU.data[3];
				RxBuff[3] = RxPDU.data[4];
				RxBuff[4] = RxPDU.data[5];
				RxBuff[5] = RxPDU.data[6];
				RxBuff[6] = RxPDU.data[7];

				if (RxParam.CompletedDataNumber + 7 < RxParam.TotalDataNumber)
				{
					CurrFrameLength = 7;
				}
				else
				{
					CurrFrameLength = RxParam.TotalDataNumber - RxParam.CompletedDataNumber;
				}

				N_USData_indication(CF, RxPDU.Mtype, RxPDU.N_SA, RxPDU.N_TA, RxPDU.N_TAtype, RxPDU.N_AE, RxBuff, CurrFrameLength, N_Ret);
			}
			else
			{
				//printf("RX end,ignore CF\r\n");
			}
		}
		else
		{
			//printf("CF error SN expect %d but %d\r\n", RxParam.SN + 1,RxPDU.PDU_L);
			N_Ret = N_WRONG_SN;
			N_USData_indication(CF, RxPDU.Mtype, RxPDU.N_SA, RxPDU.N_TA, RxPDU.N_TAtype, RxPDU.N_AE, RxBuff, CurrFrameLength, N_Ret);
		}
	}
	else
	{
		/* code */
	}
}

/****************************************
***	名称：	FlyNetwork_RecvFC
***	描述：  对流控数据进行处理
*** 备注：  
****************************************/
static void FlyNetwork_RecvFC(void)
{
	if (RxPDU.DLC < 3)
		return;

	if (gNetworkStatus == NWL_TRANSMITTING || gNetworkStatus == NWL_WAIT)
	{
		if (RxPDU.DLC == 8)
		{
			if ((RxPDU.STmin >= 0x80 && RxPDU.STmin <= 0xF0) || (RxPDU.STmin >= 0xFA))
			{
				TxPDU.STmin = 0x7F;
			}
			else if (RxPDU.STmin >= 0xF1 && RxPDU.STmin <= 0xF9)
			{
				TxPDU.STmin = 0x1;
			}
			else
			{
				TxPDU.STmin = RxPDU.STmin;
			}
			//N_USData_indication(FC, m_AddressFormat.Mtype, m_AddressFormat.N_SA, m_AddressFormat.N_TA, m_AddressFormat.N_TAtype, m_AddressFormat.N_AE, (void *)0, 0, N_Ret);
		}
		else
		{
			printf("FC invalid DLC:%d\r\n", RxPDU.DLC);
		}
	}
	else
	{
		m_TimePeriod.FC_RxBeforeFFOnSender = TRUE;
		N_Ret = N_TIMEOUT_Bs;
		//N_USData_confirm(FF, m_AddressFormat.Mtype, m_AddressFormat.N_SA, m_AddressFormat.N_TA, m_AddressFormat.N_TAtype, m_AddressFormat.N_AE, N_Ret);
		printf("not in TX or wait ,ignore FC \r\n");
	}
}

/****************************************
***	名称：	FlyPDUNetwork_RecvBufFromLinkLayer
***	描述： 	对链路层的数据进行数据判断
*** 备注：  
****************************************/
static void FlyPDUNetwork_RecvBufFromLinkLayer(ePCIType N_PDU_Type)
{
	switch (N_PDU_Type)
	{
	case SF:
		FlyNetwork_RecvSF();
		break;
	case FF:
		FlyNetwork_RecvFF();
		break;
	case CF:
		FlyNetwork_RecvCF();
		break;
	case FC:
		FlyNetwork_RecvFC();
		break;
	default:
		N_Ret = N_UNEXP_PDU;
		break;
	}
}

static void FlyPDUNetwork_SendtoLinkLayer(eTranMode tMode, uint8_t *buf, uint8_t len)
{
	switch (tMode)
	{
	case CAN_BUS:
		N_Ret = N_OK;
		Diagnose_Communication_CAN_Send(buf, len);
		break;
	case LIN_BUS:
		break;
	default:
		break;
	}

	//N_USData_confirm(txPDU.N_PciType, m_AddressFormat.Mtype, m_AddressFormat.N_SA, m_AddressFormat.N_TA, m_AddressFormat.N_TAtype, m_AddressFormat.N_AE, N_Ret);
}

static void N_USData_indication(ePCIType PciType, eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, uint8_t *MessageData, uint16_t Length, N_RESULT_INFO N_Result)
{
	uint8_t i;
	NetworkNotification notify;

	if (PciType == FC)
	{
		if (N_Ret == N_OK)
		{
			if (gTxStep == TX_WAIT_FC)
			{
				////printk("Rx FC type : %d\r\n",TxParam.FS_Type);
				TxParam.CompletedNumberInBlock = 0;
				if (TxParam.FS_Type == CTS)
				{
					gNetworkStatus = NWL_TRANSMITTING;
					m_TimePeriod.FC_RecivedOnSender = TRUE;
				}
				else if (TxParam.FS_Type == WT)
				{
					gNetworkStatus = NWL_WAIT;
					//DiagTimer_Set(&SendTimer, m_TimePeriod.N_Bs);
					printf("Rx Fs wait\r\n");
				}
				else if (TxParam.FS_Type == OVFLW)
				{
					printf("rx fs overflow,terminal transmitr\n");
					NetworkLayer_TxEnd();
				}
			}
		}
		else
		{
			NetworkLayer_TxEnd();
		}
	}
	else if (PciType == CF)
	{
		if (N_Ret == N_OK)
		{
			RxParam.CompletedNumberInBlock++;
			m_TimePeriod.CF_RecivedOnReciver = TRUE;
			for (i = 0; i < Length; i++)
			{
				NetworkPduRecvBuf[RxParam.CompletedDataNumber++] = *(MessageData + i);
			}

			if (RxParam.CompletedDataNumber == RxParam.TotalDataNumber)
			{
				notify.NotificationType = INDICATION;
				notify.Mtype = Mtype;
				notify.N_SA = N_SA;
				notify.N_TA = N_TA;
				notify.N_TAtype = N_TAtype;
				notify.N_AE = N_AE;
				notify.N_Result = N_Result;
				notify.MessageData = NetworkPduRecvBuf;
				notify.length = RxParam.TotalDataNumber;
				notify.valid = TRUE;
				Diagnose_Service_Recv_Handle(notify);
				NetworkLayer_RxEnd();
			}
		}
	}
	else if (PciType == FF)
	{
		RxParam.TotalDataNumber = Length;
		NetworkLayer_RxStart();
		for (i = 0; i < 6; i++)
			NetworkPduRecvBuf[RxParam.CompletedDataNumber++] = *(MessageData + i);
	}
	else if (PciType == SF)
	{
		if (N_Ret == N_UNEXP_PDU)
		{
			NetworkLayer_RxEnd();
		}

		notify.NotificationType = INDICATION;
		notify.Mtype = Mtype;
		notify.N_SA = N_SA;
		notify.N_TA = N_TA;
		notify.N_TAtype = N_TAtype;
		notify.N_AE = N_AE;
		notify.N_Result = N_Result;
		notify.MessageData = MessageData;
		notify.length = Length;
		notify.valid = TRUE;

		Diagnose_Service_Recv_Handle(notify);
	}
}

static void N_USData_FF_indication(eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, uint16_t Length)
{
	NetworkNotification notify;
	notify.NotificationType = FF_INDICATION;
	notify.Mtype = Mtype;
	notify.N_SA = N_SA;
	notify.N_TA = N_TA;
	notify.N_TAtype = N_TAtype;
	notify.N_AE = N_AE;
	notify.valid = TRUE;
	notify.length = Length;
	Diagnose_Service_Recv_Handle(notify);
}

static void N_USData_confirm(ePCIType PciType, eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, N_RESULT_INFO N_Result) //interface confirm to upper layer
{
	if (PciType == FF)
	{
		m_TimePeriod.FF_ConfirmedOnSender = TRUE;
	}
	else if (PciType == CF)
	{
		TxParam.CompletedNumberInBlock++;
		m_TimePeriod.CF_ConfirmedOnSender = TRUE;

		if (TxParam.TotalDataNumber == TxParam.CompletedDataNumber)
		{
			NetworkNotification notify;
			notify.NotificationType = CONFIRM;
			notify.Mtype = Mtype;
			notify.N_SA = N_SA;
			notify.N_TA = N_TA;
			notify.N_TAtype = N_TAtype;
			notify.N_AE = N_AE;
			notify.N_Result = N_Result;
			notify.valid = TRUE;
			Diagnose_Service_Recv_Handle(notify);
		}
	}
	else if (PciType == FC)
	{
		if (gRxStep == RX_WAIT_FC_CONF)
		{
			m_TimePeriod.FC_ConfirmedOnReciver = TRUE;
		}
		else
		{
			//printk("send FC on wrong time\r\n");
		}
	}
	else if (PciType == SF)
	{
		NetworkNotification notify;
		notify.NotificationType = CONFIRM;
		notify.Mtype = Mtype;
		notify.N_SA = N_SA;
		notify.N_TA = N_TA;
		notify.N_TAtype = N_TAtype;
		notify.N_AE = N_AE;
		notify.N_Result = N_Result;
		notify.valid = TRUE;
		Diagnose_Service_Recv_Handle(notify);
	}
}

void N_USData_request(eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, uint8_t *MsgBuf, uint16_t len)
{
	if (len < 8)
	{

		FlyPDUNetwork_SF_SendtoLink(MsgBuf, len);
		return;
	}

	FlyPDUNetwork_FF_SendtoLink(MsgBuf, len);
	NetworkLayer_TxStart();
}

static void FlyPDUNetwork_Send_FrametoService(u8 *buf, u8 len)
{
	Diagnose_Communication_Recive(buf, len);
}

void FlyPDUNetwork_Recv_FrameFromService(u8 *buf, u8 len)
{
}

static void FlyPDUNetwork_FC_SendtoLink(u8 status)
{
	u8 buf[8] = {0x30, 0, 0, 0, 0, 0, 0, 0};

	switch (status)
	{
	case pdu_CF_CTS:
		buf[0] = 0x30;
		break;
	case pdu_CF_WT:
		buf[0] = 0x31;
		break;
	case pdu_CF_OVFLW:
		buf[0] = 0x32;
		break;
	default:
		return;
	}

	TxPDU.N_PciType = FC;

	TxPDU.data[2] = TxPDU.STmin;

	buf[1] = defN_USdata_BS;
	buf[2] = gCFrameBTmin;

	FlyPDUNetwork_SendtoLinkLayer(CAN_BUS, buf, 8);
}

static void FlyPDUNetwork_SF_SendtoLink(u8 *data, u32 len)
{
	if (len > 7)
		return;

	memcpy(TxPDU.data + 1, data, len);
	TxPDU.N_PciType = SF;
	TxPDU.PDU_L = len;
	TxPDU.data[0] = TxPDU.N_PciType << 4 | (TxPDU.PDU_L & 0xF);

	FlyPDUNetwork_SendtoLinkLayer(CAN_BUS, TxPDU.data, TxPDU.PDU_L);
}

static void FlyPDUNetwork_FF_SendtoLink(u8 *data, u32 len)
{
//	u8 buf[8] = {0x10, 0, 0, 0, 0, 0, 0, 0};

	if (len > 7 && len < 0xFFF)
	{

		TxPDU.N_PciType = FF;
		TxPDU.PDU_L = len;

		TxPDU.data[0] = ((uint8_t)TxPDU.N_PciType << 4) | (TxPDU.PDU_L >> 8);
		TxPDU.data[1] = TxPDU.PDU_L & 0xFF;
		memcpy(TxPDU.data + 2, data, 6);

		TxParam.SN = 0;
		TxParam.TotalDataNumber = len;
		TxParam.CompletedDataNumber = 6;
		pCFData = data + 6;

		FlyPDUNetwork_SendtoLinkLayer(CAN_BUS, TxPDU.data, 8);
	}
}

static void FlyPDUNetwork_CF_SendtoLink(u8 SN, u8 *data, u32 len)
{
	uint8_t length;
	if (TxParam.CompletedDataNumber + 7 <= TxParam.TotalDataNumber)
	{
		length = 7;
	}
	else
	{
		length = TxParam.TotalDataNumber - TxParam.CompletedDataNumber;
	}
	TxPDU.data[0] = ((uint8_t)TxPDU.N_PciType << 4) | ((++TxParam.SN) & 0xF);
	TxPDU.N_PciType = CF;

	TxParam.CompletedDataNumber += length;
	pCFData += length;

	memcpy(TxPDU.data + 1, data, len);

	FlyPDUNetwork_SendtoLinkLayer(CAN_BUS, TxPDU.data, 8);
}

static void Handle_N_USdataSend(void)
{
	switch (N_USdataSend.Status)
	{
	case pdu_NormalFrame:
		break;
	case pdu_FirstFrame:
		N_USdataSend.CF_Status = pdu_CF_WT;
		N_USdataSend.Status = pdu_WaitFlowControl;
		ResetUserTimer(&N_USdataSend.Timer_BS);
		break;
	case pdu_ConsecutiveFrame:

		if (ReadUserTimer(&N_USdataSend.CF_timer) > N_USdataSend.CF_STmin)
		{
			//	printf("\r\nCFCF %x %x",ReadUserTimer(&N_USdataSend.CF_timer),N_USdataSend.CF_STmin);

			ResetUserTimer(&N_USdataSend.CF_timer);
			if ((N_USdataSend.RT_Number * 7 + 6) >= N_USdataSend.len)
			{
				FlyPDUNetwork_CF_SendtoLink(N_USdataSend.RT_Number, N_USdataSend.data + N_USdataSend.RT_Number * 7 - 1,
											(N_USdataSend.len - N_USdataSend.RT_Number * 7 + 1));
			}
			else
			{
				FlyPDUNetwork_CF_SendtoLink(N_USdataSend.RT_Number, N_USdataSend.data + N_USdataSend.RT_Number * 7 - 1, 7);
			}
			N_USdataSend.RT_Number++;
			if (((++N_USdataSend.FlowControl_RTBS) >= N_USdataSend.FlowControl_BS) && N_USdataSend.FlowControl_BS != 0)
			{
				N_USdataSend.Status = pdu_WaitFlowControl;
				N_USdataSend.CF_Status = pdu_CF_WT;
				ResetUserTimer(&N_USdataSend.Timer_BS);
			}
		}
		break;
	case pdu_WaitFlowControl:
		if (N_USdataSend.CF_Status == pdu_CF_CTS)
		{

			N_USdataSend.FlowControl_RTBS = 0;
			N_USdataSend.Status = pdu_ConsecutiveFrame;
			return;
		}
		if (N_USdataSend.CF_Status == pdu_CF_OVFLW)
		{

			return;
		}
		if (ReadUserTimer(&N_USdataSend.Timer_BS) > 1000)
		{
			printf("\r\n##WaitFlowControl outtime");
		}

		break;
	}
}

/****************************************
***	名称：	FlyPDUNetwork_Init
***	描述：	对pdu等进行初始化 
*** 参数：	void
*** 返回：  void
****************************************/
void FlyPDUNetwork_Init(void)
{

	m_TimePeriod.N_As = 25;
	m_TimePeriod.N_Ar = 25;
	m_TimePeriod.N_Br = 50;
	m_TimePeriod.N_Bs = 75;
	m_TimePeriod.N_Cs = 25;
	m_TimePeriod.N_Cr = 150;

	TxParam.FS_Type = CTS;
	TxParam.BlockSize = 20;
	TxParam.CompletedNumberInBlock = 0;
	TxParam.STmin = 100;
	TxParam.SN = 0;
	TxParam.TotalDataNumber = 100;
	TxParam.CompletedDataNumber = 0;

	RxParam.FS_Type = CTS;
	RxParam.BlockSize = 0;
	RxParam.CompletedNumberInBlock = 0;
	RxParam.STmin = 0x0A;
	RxParam.SN = 0;
	RxParam.TotalDataNumber = 0;
	RxParam.CompletedDataNumber = 0;
	RxParam.BuffSize = 100;
	pCFData = NULL;
}

/****************************************
***	名称：	FlyPDUNetwork_Pro
***	描述：  诊断通讯的处理过程
*** 备注：  需要轮询调用
****************************************/
static void FlyPDUNetwork_Main_Pro(void)
{
	if (gNetworkStatus == NWL_TRANSMITTING) //sending 发送数据中
	{
		if (gTxStep == TX_WAIT_FF_CONF) //发送首帧
		{
			if (m_TimePeriod.FF_ConfirmedOnSender == TRUE)
			{
				gTxStep = TX_WAIT_FC;
				m_TimePeriod.FF_ConfirmedOnSender = FALSE;
			}
			else
			{

				NetworkLayer_TxEnd(); //time out end
				N_Ret = N_TIMEOUT_A;
				printf("timeout wait FF conf\r\n");
				//N_USData_confirm(FF, m_AddressFormat.Mtype, m_AddressFormat.N_SA, m_AddressFormat.N_TA, m_AddressFormat.N_TAtype, m_AddressFormat.N_AE, m_N_Result);
			}
		}
		else if (gTxStep == TX_WAIT_FC)
		{
			//if (DiagTimer_HasExpired(&CFRequestTimer)) //when first CF or wait STmin end
			{
				//NetworkLayer_SendCF();
				gTxStep = TX_WAIT_CF_CONF;
				//DiagTimer_Set(&SendTimer, m_TimePeriod.N_As);
			}
		}
		else if (gTxStep == TX_WAIT_CF_REQ)
		{
		}
		else if (gTxStep == TX_WAIT_CF_CONF)
		{
		}
	}
	else if (gNetworkStatus == NWL_RECIVING)
	{
		if (gRxStep == RX_WAIT_FC_REQ)
		{
		}
		else if (gRxStep == RX_WAIT_FC_CONF)
		{
		}
		else if (gRxStep == RX_WAIT_CF)
		{
		}
	}
	else if (gNetworkStatus == NWL_WAIT)
	{
		if (gTxStep == TX_WAIT_FC)
		{
		}
	}
}

/****************************************
***	名称：	FlyPDUNetwork_Pro
***	描述：  诊断通讯的处理过程
*** 备注：  需要轮询调用
****************************************/
void FlyPDUNetwork_Pro(void)
{
	Handle_N_USdataSend();
	FlyPDUNetwork_Main_Pro();
}
