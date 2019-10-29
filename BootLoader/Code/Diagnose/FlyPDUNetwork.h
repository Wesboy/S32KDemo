
#ifndef ___PDU_NETWORK__H_
#define ___PDU_NETWORK__H_

#include <string.h>
#include <stdio.h>
#include "interface.h"

typedef unsigned int u32, U32;
typedef unsigned char u8, U8;
typedef unsigned short int u16, U16;

typedef enum
{
	N_OK,
	N_TIMEOUT_A,
	N_TIMEOUT_Bs,
	N_TIMEOUT_Cr,
	N_WRONG_SN,
	N_INVALID_FS,
	N_UNEXP_PDU,
	N_WFT_OVRN,
	N_BUFFER_OVFLW,
	N_EEROR
} N_RESULT_INFO;


#define N_USdata_Len 255 //ISO15765????? 0xFF
#define defN_USdata_BS 0
#define defN_USdata_STmin 120

#define CAN_ID_DIAGNOSIS_FUNCTION 0x7DF
#define CAN_ID_DIAGNOSIS_SEND 0x8DF

#define DiagnosticSessionConTrolService (0x10)
#define ECUResetService (0x11)
#define SecurityAccessService (0x27) 
#define CommunicationControlservice (0x28)
#define TesterPresentservice (0x3E)
#define AccessTimingParameterservice (0x83)
#define SecuredDataTransmissionservice (0x84)
#define ControlDTCSettingservice (0x85)
#define ResponseOnEventservice (0x86)
#define LinkControlservice (0x87)
#define ReadDataByIdentifierservice (0x22)
#define ReadMemoryByAddressservice (0x23)
#define ReadScalingDataByIdentifierservice (0x24)
#define ReadDataByPeriodicIdentifierservice (0x2A)
#define DynamicallyDefineDataIdentifierservice (0x2C)
#define WriteDataByIdentifierservice (0x2E)
#define WriteMemoryByAddressservice (0x3D)
#define ClearDiagnosticInformationService (0x14)
#define ReadDTCInformationService (0x19)
#define InputOutputControlByIdentifierservice (0x2F)
#define RoutineControlservice (0x31)
#define RequestDownloadservice (0x34)
#define RequestUploadservice (0x35)
#define TransferDataservice (0x36)
#define RequestTransferExitservice (0x37)
#define RequestFileTransferservice (0x38)



enum
{
	pdu_SingleFrame = 0,
	pdu_FirstFrame,
	pdu_ConsecutiveFrame,
	pdu_FlowControl,
	pdu_WaitFlowControl,
	pdu_NormalFrame,
	pdu_N_PDU_SF = 0,
	pdu_N_PDU_FF,
	pdu_N_PDU_CF,
	pdu_N_PDU_FC,
	pdu_CF_CTS = 0,
	pdu_CF_WT,
	pdu_CF_OVFLW,
};

typedef struct
{
	u8 data[N_USdata_Len];
	u8 Status;
	u8 RT_Number;
	U8 CF_Status;
	U32 CF_STmin;
	U32 CF_timer;
	u32 len;
	u8 FlowControl_BS;
	U8 FlowControl_RTBS;
	u32 Timer_BS;

}N_USdata;


typedef enum{
    CAN_BUS,
    LIN_BUS,
    FLEXRAY_BUS,
    Ethernet
}eTranMode;


typedef enum{
	Diagnostic,			
	Remote_Diagnostic
}eMsgType;

typedef enum{
	type_phy,//????
	type_fun //????
}eNAddrType;

typedef enum{
	STmin,
	BS
}ePara;

typedef enum{
	N_ParaOK,
	N_RX_ON,
	N_WRONG_PARAMETER,
	N_WRONG_VALUE,
}eRChangePara;

typedef enum
{
	CTS,   //continue to send
	WT,		//wait
	OVFLW, //overflow
}eFS_Type;

typedef enum
{
	SF = 0, //single frame
	FF = 1, //first frame
	CF = 2, //consecutive frame
	FC = 3, //flow control
}ePCIType;

typedef enum
{
	NWL_IDLE,
	NWL_TRANSMITTING,
	NWL_RECIVING,
	NWL_WAIT,
}eNWL_Status;

struct N_time{
	u16 N_Ar;		//?????CAN????????
	u16 N_As;		//?????CAN????????Data.request????Service indicate
	u16 N_Br;		//?????????????????????
	u16 N_Bs;		//?????????????????????
	u16 N_Cr;		//?????????????????????
	u16 N_Cs;		//?????????????????????
	bool FF_ConfirmedOnSender;  //when transmission
	bool FC_RecivedOnSender;	//when transmission
	bool CF_RequestedOnSender;  //when transmission
	bool CF_ConfirmedOnSender;  //when transmission
	bool FC_RxBeforeFFOnSender; //when transmission
	bool FC_RequestedOnReciver; //when reciving
	bool FC_ConfirmedOnReciver; //when reciving
	bool CF_RecivedOnReciver;   //when reciving
};

typedef struct
{
	eMsgType Mtype;
	uint8_t N_SA;
	uint8_t N_TA;
	eNAddrType N_TAtype;
	uint8_t N_AE;
}eAddrFormat;


typedef struct
{
	eFS_Type FS_Type;
	uint8_t BlockSize;
	uint8_t CompletedNumberInBlock;
	uint8_t STmin;
	uint8_t SN;
	uint16_t TotalDataNumber;
	uint16_t CompletedDataNumber;
	uint16_t BuffSize;
} CommuParam;



typedef enum
{
	CONFIRM,
	FF_INDICATION,
	INDICATION,
} eNWNType;

typedef struct
{
	eNWNType NotificationType;
	eMsgType Mtype;
	uint8_t N_SA;
	uint8_t N_TA;
	eNAddrType N_TAtype;
	uint8_t N_AE;
	uint8_t *MessageData;
	uint16_t length;
	N_RESULT_INFO N_Result;
	bool valid;
} NetworkNotification;

typedef struct
{
	eMsgType Mtype;		 //???  ??? ??????
	u16 N_SA;			 //????????
	u16 N_TA;			 //?????????
	eNAddrType N_TAtype; //?????????????
	u8 N_AE;			 //??????????
	u8 MsgBuf[256];
	u8 len;
} sUSDataRequst;

struct usdataIndication{
	eMsgType Mtype;
	u16 N_SA;   //????????
	u16 N_TA;   //?????????
	eNAddrType N_TAtype; //?????????????
	u8 N_AE;		//??????????
	u8 MsgData;
	u8 length;	//1-4095   12bit  &0xFFF
	N_RESULT_INFO N_Result;
};

typedef enum
{
	TX_IDLE,
	TX_WAIT_FF_CONF,
	TX_WAIT_FC,
	TX_WAIT_CF_REQ,
	TX_WAIT_CF_CONF,
}eTranStep;

typedef enum
{
	RX_IDLE,
	RX_WAIT_FC_REQ,
	RX_WAIT_FC_CONF,
	RX_WAIT_CF,
}eRecvStep;

typedef struct
{
	eMsgType Mtype;
	eNAddrType N_TAtype;
	uint8_t N_SA;
	uint8_t N_TA;
	uint8_t N_AE;
	uint32_t ID;
	uint8_t DLC;
	uint8_t RTR;
	uint8_t IDE;
	bool valid;
	uint8_t data[8];
	uint8_t STmin;	 //STmin ,data2
	uint16_t PDU_L;
	ePCIType N_PciType : 4;
}N_PDU;

struct usdataConfirm
{
	eMsgType Mtype;
	u16 N_SA;			 //????????
	u16 N_TA;			 //?????????
	eNAddrType N_TAtype; //?????????????
	u8 N_AE;			 //??????????
	u8 parameter;
	u8 Result_ChangeParameter;
};



/*?????*/
extern void SendDataToSTDCAN(uint8_t *buf, uint8_t len, uint32_t CAN_id);
extern void Diagnose_Communication_Recive(unsigned char *buff, unsigned char len);
extern void Diagnose_Service_Recv_Handle(NetworkNotification notify);
/*??????*/
extern void FlyPDUNetwork_RecvFromCAN(u32 ID, u8 *buf, u8 len);
extern void FlyPDUNetwork_Init(void);
extern void FlyPDUNetwork_Pro(void);
extern void FlyPDUNetwork_Recv_FrameFromService(u8 *buf, u8 len);
extern void N_USData_request(eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, uint8_t *MessageData, uint16_t Length);

/*???????*/
static void FlyPDUNetwork_RecvBufFromLinkLayer(ePCIType N_PDU_Type);
static void FlyPDUNetwork_SendtoLinkLayer(eTranMode tMode, uint8_t *buf, uint8_t len);
static void FlyPDUNetwork_Send_FrametoService(u8 *buf, u8 len);
static void FlyPDUNetwork_FC_SendtoLink(u8 status);
static void FlyPDUNetwork_SF_SendtoLink(u8 *data, u32 len);
static void FlyPDUNetwork_FF_SendtoLink(u8 *data, u32 len);
static void FlyPDUNetwork_CF_SendtoLink(u8 SN, u8 *data, u32 len);
static void Handle_N_USdataSend(void);

static void N_USData_indication(ePCIType PciType, eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, uint8_t *MsgBuf, uint16_t Length, N_RESULT_INFO N_Result);
static void N_USData_FF_indication(eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, uint16_t Length);
static void N_USData_confirm(ePCIType PciType, eMsgType Mtype, uint8_t N_SA, uint8_t N_TA, eNAddrType N_TAtype, uint8_t N_AE, N_RESULT_INFO N_Result);//interface confirm to upper layer

#endif
