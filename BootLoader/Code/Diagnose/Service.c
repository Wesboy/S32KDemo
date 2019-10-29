/* Servce.c                              Flyaudio 2019
 * 描述: 诊断的服务内容
 *              
 * 2019 02 20 移植预研阶段的诊断升级模块
 * 
 * 
 */
#include "Servce.h"

#include <stdio.h>
#include <string.h>

#include "FlyPDUNetwork.h"
enum enumServiceType
{
	eRequestMessage = 0,
	ePostiveResponseMessage,
	eNegativeResponseCodes,
};

static sUSDataRequst gServiceReq;
/*********************************************************************************************************
** Function name:       Dianogstic_SendService
** Descriptions:        诊断服务的请求
** input parameters:    Service:          请求的诊断服务
**											buff：						诊断服务的参数
**											len：							诊断服务的长度
**											ServiceType：			诊断服务的类型：请求，正响应，否定响应
** Returned value:      NULL
*********************************************************************************************************/
static void
Dianogstic_SendService(unsigned char Service, unsigned char *buff, unsigned char len, unsigned char ServiceType)
{
	u8 SendService = 0;

	switch (ServiceType)
	{
	case eRequestMessage:
		SendService = Service;
		break;
	case ePostiveResponseMessage:
		SendService = Service + 0x40;
		break;
	case eNegativeResponseCodes:
		SendService = 0x7f;
		break;
	default:
		return;
	}

	gServiceReq.Mtype = Diagnostic;
//	gServiceReq.N_AE;
//	gServiceReq.N_SA;
//	gServiceReq.N_TA;
	gServiceReq.N_TAtype = type_phy;
	gServiceReq.len = len + 1;
	gServiceReq.MsgBuf[0] = SendService;
	memcpy(gServiceReq.MsgBuf + 1, buff, len);

	N_USData_request(gServiceReq.Mtype, gServiceReq.N_SA, gServiceReq.N_TA, gServiceReq.N_TAtype, gServiceReq.N_AE, gServiceReq.MsgBuf, gServiceReq.len);
}
/*********************************************************************************************************
** Function name:       Service_NRC
** Descriptions:        否定码发送
** input parameters:    Service					服务SID
**											NRC							故障码NRC
** Returned value:      NULL
*********************************************************************************************************/
static void Service_NRC(unsigned char Service, unsigned char NRC)
{
	unsigned char buf[2];
	buf[0] = Service;
	buf[1] = NRC;
	Dianogstic_SendService(Service, buf, 2, eNegativeResponseCodes);
}

/*********************************************************************************************************
** Function name:       DiagnosticSessionConTrolServiceDeal
** Descriptions:        诊断会话控制服务处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void DiagnosticSessionConTrolServiceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	if (len != 1)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(DiagnosticSessionConTrolService, NRC_0x13_IMLOIF);
		return;
	}
	switch (buff[0])
	{
	case fuc_defaultSession:
		//对应的请求
		Prog_SetDiagSession(fuc_defaultSession);
		break;
	case fuc_ProgrammingSession:
		//
		Prog_SetDiagSession(fuc_ProgrammingSession);
		break;
	case fuc_extendedDiagnosticSession:
		//
		Prog_SetDiagSession(fuc_extendedDiagnosticSession);
		break;
	case fuc_safetySystemDiagnosticSession:
		//
		Prog_SetDiagSession(fuc_safetySystemDiagnosticSession);
		break;
	default:
		Service_NRC(DiagnosticSessionConTrolService, NRC_0x12_SFNS);
		printf("\r\nNRC: 0x12");
		return;
	}
	para[0] = buff[0];
	para[1] = (P2server_max >> 8) & 0xff;
	para[2] = (P2server_max)&0xff;
	para[3] = (P2Nserver_max >> 8) & 0xff;
	para[4] = (P2Nserver_max)&0xff;
	Dianogstic_SendService(DiagnosticSessionConTrolService, para, 5, ePostiveResponseMessage);
	printf("\r\nPosRespMess: DiagnosticSessionConTrolService");
}
/*********************************************************************************************************
** Function name:       ECUResetServiceDeal
** Descriptions:        ECU服务服务处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void ECUResetServiceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	if (len > 2)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(ECUResetService, NRC_0x13_IMLOIF);
		return;
	}
	switch (buff[0])
	{
	case fuc_hardReset:
		//对应的请求
		break;
	case fuc_keyOffOnReset:
		//
		break;
	case fuc_softReset:
		//
		break;
	case fuc_enableRapidPowerShutDown:
		//
		break;
	case fuc_disableRapidPowerShutDown:
		break;
	default:
		Service_NRC(ECUResetService, NRC_0x12_SFNS);
		printf("\r\nNRC: 0x12");
		return;
	}
	para[0] = buff[0];
	if (fuc_enableRapidPowerShutDown != para[0])
		Dianogstic_SendService(ECUResetService, para, 1, ePostiveResponseMessage);
	else
	{
		para[1] = para_powerDownTime;
		Dianogstic_SendService(ECUResetService, para, 2, ePostiveResponseMessage);
	}
	printf("\r\nPosRespMess: ECUResetServiceDeal");
}
/*********************************************************************************************************
** Function name:       ControlDTCSettingserviceDeal
** Descriptions:        DTC控制设置服务处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void ControlDTCSettingserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	if (len < 1)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(ControlDTCSettingservice, NRC_0x13_IMLOIF);
		return;
	}
	switch (buff[0])
	{
	case fuc_eDTCSettingOn:
	case fuc_eDTCSettingOff:
		Prog_SetDTC(buff[0]);
		break;
	default:
		Service_NRC(ControlDTCSettingservice, NRC_0x12_SFNS);
		printf("\r\nNRC: 0x12");
		return;
	}
	para[0] = buff[0];
	Dianogstic_SendService(ControlDTCSettingservice, para, 1, ePostiveResponseMessage);
	printf("\r\nPosRespMess: ControlDTCSettingservice");
}
/*********************************************************************************************************
** Function name:       CommunicationControlserviceDeal
** Descriptions:        通信控制服务
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void CommunicationControlserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	if (len < 2)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(CommunicationControlservice, NRC_0x13_IMLOIF);
		return;
	}
	switch (buff[0])
	{
	case fuc_enableRxAndTx:
	case fuc_enableRxAndDisableTx:
	case fuc_DisableRxAndenableTx:
	case fuc_DisableRxAndTx:
	case fuc_enableRxAndDisableTxWithEnhancedAddressInformation:
	case fuc_enableRxAndTxWithEnhancedAddressInformation:
		Prog_CommunicationControl(buff[0]);
		//Prog_SetDTC(buff[0]);
		break;
	default:
		Service_NRC(CommunicationControlservice, NRC_0x12_SFNS);
		printf("\r\nNRC: 0x12");
		return;
	}
	para[0] = buff[0];
	Dianogstic_SendService(CommunicationControlservice, para, 1, ePostiveResponseMessage);
	printf("\r\nPosRespMess: CommunicationControlservice");
}
/*********************************************************************************************************
** Function name:       RoutineControlserviceDeal
** Descriptions:        例程控制服务处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void RoutineControlserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	unsigned char ret = 0;
	if (len < 3)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(RoutineControlservice, NRC_0x13_IMLOIF);
		return;
	}
	switch (buff[0])
	{
	case fuc_startRoutine:
	case fuc_stopRoutine:
	case fuc_requestRoutineResults:
		ret = Prog_RoutineControl(buff[0], ((unsigned short int)buff[1] << 8) + buff[2]);
		break;
	default:
		ret = NRC_0x12_SFNS;

		return;
	}
	para[0] = buff[0];
	if (ret)
	{
		Service_NRC(RoutineControlservice, ret);
		printf("\r\nNRC: 0x%02x", ret);
		return;
	}
	Dianogstic_SendService(RoutineControlservice, para, 1, ePostiveResponseMessage);
	printf("\r\nPosRespMess: RoutineControlservice");
}
/*********************************************************************************************************
** Function name:       ClearDiagnosticInformationServiceDeal
** Descriptions:        清除诊断服务
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void ClearDiagnosticInformationServiceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	unsigned char ret = 0;
	if (len != 4)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(ClearDiagnosticInformationService, NRC_0x13_IMLOIF);
		return;
	}
	para[0] = buff[0];
	if (ret)
	{
		Service_NRC(ClearDiagnosticInformationService, ret);
		printf("\r\nNRC: 0x%02x", ret);
		return;
	}
	Dianogstic_SendService(ClearDiagnosticInformationService, para, 1, ePostiveResponseMessage);
	printf("\r\nPosRespMess: ClearDiagnosticInformationService");
}
/*********************************************************************************************************
** Function name:       RequestDownloadserviceDeal
** Descriptions:        下载数据请求服务
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void RequestDownloadserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	unsigned char ret = 0;
	if (len < 2)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(RequestDownloadservice, NRC_0x13_IMLOIF);
		return;
	}
	para[0] = buff[0];
	ret = Prog_RequestDownloadservice(buff, len, para);
	if (ret)
	{
		Service_NRC(RequestDownloadservice, ret);
		printf("\r\nNRC: 0x%02x", ret);
		return;
	}
	Dianogstic_SendService(RequestDownloadservice, para, 1 + (para[0] >> 4), ePostiveResponseMessage);
	printf("\r\nPosRespMess: RequestDownloadservice");
}
/*********************************************************************************************************
** Function name:       RequestUploadserviceDeal
** Descriptions:       	上传数据服务处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void RequestUploadserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	unsigned char ret = 0;
	ret = Prog_RequestUploadserviceservice(buff, len, para);
	if (ret)
	{
		Service_NRC(RequestUploadservice, ret);
		printf("\r\nNRC: 0x%02x", ret);
		return;
	}
	Dianogstic_SendService(RequestUploadservice, para, 1 + (para[0] >> 4), ePostiveResponseMessage);
	printf("\r\nPosRespMess: RequestUploadservice");
}
/*********************************************************************************************************
** Function name:       RequestTransferExitserviceDeal
** Descriptions:        停止数据传输服务
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void RequestTransferExitserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	unsigned char ret = 0;
	para[0] = buff[0];
	ret = Prog_RequestTransferExitservice(buff, len);
	if (ret)
	{
		Service_NRC(RequestTransferExitservice, ret);
		printf("\r\nNRC: 0x%02x", ret);
		return;
	}
	Dianogstic_SendService(RequestTransferExitservice, para, 1, ePostiveResponseMessage);
	printf("\r\nPosRespMess: RequestTransferExitservice");
}
/*********************************************************************************************************
** Function name:       TransferDataserviceDeal
** Descriptions:        数据传输服务处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
static void TransferDataserviceDeal(unsigned char *buff, unsigned char len)
{
	unsigned char para[10];
	unsigned char ret = 0;
	if (len == 0)
	{
		printf("\r\nNRC: 0x13");
		Service_NRC(TransferDataservice, NRC_0x13_IMLOIF);
		return;
	}
	para[0] = buff[0];
	ret = Prog_TransferDataservice(buff, len);
	if (ret)
	{
		Service_NRC(TransferDataservice, ret);
		printf("\r\nNRC: 0x%02x", ret);
		return;
	}
	Dianogstic_SendService(TransferDataservice, para, 1, ePostiveResponseMessage);
	printf("\r\nPosRespMess: TransferDataservice");
}
/*********************************************************************************************************
** Function name:       Dianogstic_ReceiveService
** Descriptions:        CAN诊断数据的接受入口，接受后对数据进行处理
** input parameters:    buff						诊断参数
**											len							诊断参数长度
** Returned value:      NULL
*********************************************************************************************************/
void Diagnose_Communication_Recive(unsigned char *buff, unsigned char len)
{
	unsigned char i;

	printf("\r\nDianogstic_ReceiveService:");
	for (i = 0; i < len; i++)
	{
		printf(" %02x", buff[i]);
	}

	//SID ????ж? ??

	switch (buff[0])
	{
	case DiagnosticSessionConTrolService:
		DiagnosticSessionConTrolServiceDeal(buff + 1, len - 1);
		break;
	case ECUResetService:
		ECUResetServiceDeal(buff + 1, len - 1);
		break;
	case RequestDownloadservice:
		RequestDownloadserviceDeal(buff + 1, len - 1);
		break;
	case RequestUploadservice:
		RequestUploadserviceDeal(buff + 1, len - 1);
		break;
	case TransferDataservice:
		TransferDataserviceDeal(buff + 1, len - 1);
		break;
	case RequestTransferExitservice:
		RequestTransferExitserviceDeal(buff + 1, len - 1);
		break;
	case RequestFileTransferservice:
		break;
	case ControlDTCSettingservice:
		ControlDTCSettingserviceDeal(buff + 1, len - 1);
		break;
	case CommunicationControlservice:
		CommunicationControlserviceDeal(buff + 1, len - 1);
		break;
	case RoutineControlservice:
		RoutineControlserviceDeal(buff + 1, len - 1);
		break;
	case ClearDiagnosticInformationService:
		ClearDiagnosticInformationServiceDeal(buff + 1, len - 1);
		break;
	}
}

/*********************************************************************************************************
** Function name:       Diagnose_Service_Recv_Handle
** Descriptions:       
** input parameters:    
** Returned value:      NULL
*********************************************************************************************************/
void Diagnose_Service_Recv_Handle(NetworkNotification notify)
{
	if (notify.NotificationType == INDICATION)
	{
		if (notify.N_Result == N_OK || notify.N_Result == N_UNEXP_PDU)
		{
			Diagnose_Communication_Recive(notify.MessageData, notify.length);
		}
	}
	else if (notify.NotificationType == CONFIRM)
	{
	}
	else if (notify.NotificationType == FF_INDICATION)
	{
	}
}

/*********************************************************************************************************
** Function name:       Diagnose_Service_Main_Pro
** Descriptions:       
** input parameters:   		
** Returned value:      NULL
*********************************************************************************************************/
void Diagnose_Service_Main_Pro(void)
{

	return;
}
