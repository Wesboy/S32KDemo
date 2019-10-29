
#ifndef ___DIAGNOSE_COMMUNICATION__H_
#define ___DIAGNOSE_COMMUNICATION__H_



typedef  unsigned int  				u32,U32;
typedef  unsigned char  			u8,U8;
typedef  unsigned short  int 	u16,U16;


void Diagnose_Communication_Init(void);										//主程序调用
void Diagnose_Communication_Request(u8 *data,u32 len);		//Sevice.c调用
void Diagnose_Communication_CAN_Send(u8 *data,u8 len);		//主程序实现其函数
void Diagnose_Communication_Pro(void);										//主程序调用
void Diagnose_Communication_CAN_Receive(u8 *data,u8 len);	//主程序调用
void Diagnose_Communication_Recive(u8 *data,u8 len);			//Sevice.c实现其函数
void SetCFrameBTmin(const unsigned  int val);             //主程序调用



u32 ReadUserTimer(const u32 * const Timer);
void ResetUserTimer(u32 * const Timer);

#define DiagnosticSessionConTrolService   						(0x10)
#define ECUResetService																(0x11)
#define SecurityAccessService													(0x27)//安全接入服务
#define CommunicationControlservice 									(0x28) 
#define TesterPresentservice  												(0x3E) 
#define AccessTimingParameterservice  								(0x83) 
#define SecuredDataTransmissionservice    						(0x84) 
#define ControlDTCSettingservice    									(0x85) 
#define ResponseOnEventservice   											(0x86) 
#define LinkControlservice 														(0x87) 
#define ReadDataByIdentifierservice   								(0x22) 
#define ReadMemoryByAddressservice   									(0x23) 
#define ReadScalingDataByIdentifierservice    				(0x24) 
#define ReadDataByPeriodicIdentifierservice    				(0x2A) 
#define DynamicallyDefineDataIdentifierservice 				(0x2C) 
#define WriteDataByIdentifierservice    							(0x2E) 
#define WriteMemoryByAddressservice 									(0x3D) 
#define ClearDiagnosticInformationService    					(0x14) 
#define ReadDTCInformationService 										(0x19) 
#define InputOutputControlByIdentifierservice 				(0x2F) 
#define RoutineControlservice 												(0x31) 
#define RequestDownloadservice 												(0x34) 
#define RequestUploadservice 													(0x35) 
#define TransferDataservice 													(0x36) 
#define RequestTransferExitservice 										(0x37) 
#define RequestFileTransferservice 										(0x38) 
#endif
