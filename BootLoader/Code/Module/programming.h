#ifndef ___PROGRAMMING__H_
#define ___PROGRAMMING__H_


void Programming_Init(void);
void ProgramPro(void);
void Prog_SetDiagSession(unsigned char Session);
void Prog_SetDTC(unsigned char DTC);
void Prog_CommunicationControl(unsigned char Communication);
unsigned char Prog_RoutineControl(unsigned char Routine,unsigned short int DID);
unsigned char Prog_RequestDownloadservice(unsigned char *buff,unsigned char len,unsigned char *);
unsigned char Prog_RequestTransferExitservice(unsigned char *buff,unsigned char len);
unsigned char Prog_TransferDataservice(unsigned char *buff,unsigned char len);
unsigned char Prog_RequestUploadserviceservice(unsigned char *buff,unsigned char len,unsigned char *);
void BootorUser(void);


#endif

