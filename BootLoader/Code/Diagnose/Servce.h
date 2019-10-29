#ifndef ___SERVICE__H_
#define ___SERVICE__H_

#include <stdint.h>
#include <stdbool.h>

/********          ���º�����������Ҫ�����嶨��       *************************/
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
/*******************************************************************************/
#define P2server_max 			50//ms
#define P2Nserver_max 		500//ms

#define NRC_0x12_SFNS    	0X12//sub-functionNoSupported
#define NRC_0x13_IMLOIF		0x13//inorrectMessageLengthOrInvalidFormat
#define NRC_0x22_CNC			0x22//conditonsNotCorrect
#define NRC_0x24_RSE			0x24//requestSequenceError
#define NRC_0x73_WBSC			0x73//wrongBlockSequenceCounter
#define NRC_0x31_ROOR 		0x31//requestOutOfRange
#define NRC_0x33_SAD			0x33//securityAccessDenied
#define NRC_0x70_UDNA			0x70//uploadDownloadNotAccepted
#define NRC_0x72_GPF			0x72//GeneralProgrammingFailure

#define fuc_defaultSession								0x1
#define fuc_ProgrammingSession						0x2
#define fuc_extendedDiagnosticSession			0x3
#define fuc_safetySystemDiagnosticSession 0x4

#define fuc_hardReset											0x1
#define fuc_keyOffOnReset									0x2
#define fuc_softReset											0x3
#define fuc_enableRapidPowerShutDown			0x4
#define fuc_disableRapidPowerShutDown			0x5
#define para_powerDownTime 								5
#define maxNumberOfBlockLength_34					16
#define lengthFormatIdentifier_34					0x10

#define fuc_eDTCSettingOn			0x1
#define fuc_eDTCSettingOff 		0x2

#define fuc_enableRxAndTx			0x00
#define fuc_enableRxAndDisableTx			0x01
#define fuc_DisableRxAndenableTx			0x02
#define fuc_DisableRxAndTx			0x03
#define fuc_enableRxAndDisableTxWithEnhancedAddressInformation			0x04
#define fuc_enableRxAndTxWithEnhancedAddressInformation			0x05


#define fuc_startRoutine	0x1
#define fuc_stopRoutine	0x2
#define fuc_requestRoutineResults 0x3






#define reportNumberOfDTCByStatusMask 0x1
#define reportDTCByStatusMask	0x2
#define reportDTCSnapshotldenttification	0x3
#define	reportDTCSnapshotRecordByDTCNumber	0x4
#define reportDTCStoredDataByRecordNumber 0x5
#define reportDTCExtDataRecordByDTCNumber	0x6
#define reportNumberOfDTCBySeverityMaskRecord 0x7
#define reportDTCBySeverityMaskRecord 0x8
#define reportSeverityInformationOfDTC 0x9
#define reportSupportedDTC 0xa
#define reportFirstTestFailedDTC 0xb
#define reportFirstConfirmedDTC 0xc
#define reportMostRecentTestFailedDTC 0xd
#define reportMostRecentConfirmedDTC 0xe
#define reportMirrorMemoryDTCByStatusMask 0xf
#define reportMirrorMemoryDTCExtDataRecordByDTCNumber 0x10
#define reportNumberOfMirrorMemoryDTCByStatusMask 0x11
#define reportNumberOfEmissionsOBDDTCByStatusMask 0x12
#define reportEmissionsOBDDTCByStatusMask 0x13
#define reportDTCFaultDetectionCounter 0x14
#define reportDTCWithPermanentStatus 0x15
#define reportDTCExtDataRecordByRecordNumber 0x16
#define reportUserDefMemoryDTCByStatusMask 0x17
#define reportUserDefMemoryDTCSnapshotRecordByDTCNumber 0x18
#define reportUserDefMemoryDTCExtDataRecordByDTCNumber 0x19
#define reportWWHOBDDTCByMaskRecord 0x42
#define reportWWHOBDDTCWithPermanentStatus 0x55

// TODO This still doesn't have enough space for the largest possible
// multiframe response. May need to dynamically allocate in the future.
#define MAX_UDS_RESPONSE_PAYLOAD_LENGTH 127
#define MAX_UDS_REQUEST_PAYLOAD_LENGTH 7
#define MAX_RESPONDING_ECU_COUNT 8
#define VIN_LENGTH 17

/* Private: The four main types of diagnositc requests that determine how the
 * request should be parsed and what type of callback should be used.
 *
 * TODO this may not be used...yet?
 */
typedef enum
{
    DIAGNOSTIC_REQUEST_TYPE_PID,
    DIAGNOSTIC_REQUEST_TYPE_DTC,
    DIAGNOSTIC_REQUEST_TYPE_MIL_STATUS,
    DIAGNOSTIC_REQUEST_TYPE_VIN
} DiagnosticRequestType;

/* Public: A container for a single diagnostic request.
 *
 * The only required fields are the arbitration_id and mode.
 *
 * arbitration_id - The arbitration ID to send the request.
 * mode - The OBD-II mode for the request.
 * has_pid - (optional) If the requests uses a PID, this should be true.
 * pid - (optional) The PID to request, if the mode requires one. has_pid must
 *      be true.
 * pid_length - The length of the PID field, either 1 (standard) or 2 bytes
 *      (extended). If 0, it will be set automatically based on the request
 *      mode.
 * payload - (optional) The payload for the request, if the request requires
 *      one. If payload_length is 0 this field is ignored.
 * payload_length - The length of the payload, or 0 if no payload is used.
 * no_frame_padding - false if sent CAN payloads should *not* be padded out to a
 *      full 8 byte CAN frame. Many ECUs require this, but others require the
 *      size of the CAN message to only be the actual data. By default padding
 *      is enabled (so this struct value can default to 0).
 * type - the type of the request (TODO unused)
 */
typedef struct
{
    uint32_t arbitration_id;
    uint8_t mode;
    bool has_pid;
    uint16_t pid;
    uint8_t pid_length;
    uint8_t payload[MAX_UDS_REQUEST_PAYLOAD_LENGTH];
    uint8_t payload_length;
    bool no_frame_padding;
    DiagnosticRequestType type;
} DiagnosticRequest;

/* Public: All possible negative response codes that could be received from a
 * requested node.
 *
 * When a DiagnosticResponse is received and the 'completed' field is true, but
 * the 'success' field is false, the 'negative_response_code' field will contain
 * one of these values as reported by the requested node.
 *
 * Thanks to canbushack.com for the list of NRCs.
 */
typedef enum
{
    NRC_SUCCESS = 0x0,
    NRC_SERVICE_NOT_SUPPORTED = 0x11,
    NRC_SUB_FUNCTION_NOT_SUPPORTED = 0x12,
    NRC_INCORRECT_LENGTH_OR_FORMAT = 0x13,
    NRC_CONDITIONS_NOT_CORRECT = 0x22,
    NRC_REQUEST_OUT_OF_RANGE = 0x31,
    NRC_SECURITY_ACCESS_DENIED = 0x33,
    NRC_INVALID_KEY = 0x35,
    NRC_TOO_MANY_ATTEMPS = 0x36,
    NRC_TIME_DELAY_NOT_EXPIRED = 0x37,
    NRC_RESPONSE_PENDING = 0x78
} DiagnosticNegativeResponseCode;

/* Public: A partially or fully completed response to a diagnostic request.
 *
 * completed - True if the request is complete - some functions return a
 *      DiagnosticResponse even when it's only partially completed, so be sure
 *      to check this field.
 * success - True if the request was successful. The value if this
 *      field isn't valid if 'completed' isn't true. If this is 'false', check
 *      the negative_response_code field for the reason.
 * arbitration_id - The arbitration ID the response was received on.
 * multi_frame - True if this response (whether completed or not) required
 *      multi-frame CAN support. Can be used for updating time-out functions.
 * mode - The OBD-II mode for the original request.
 * has_pid - If this is a response to a PID request, this will be true and the
 *      'pid' field will be valid.
 * pid - If the request was for a PID, this is the PID echo. Only valid if
 *      'has_pid' is true.
 * negative_response_code - If the request was not successful, 'success' will be
 *      false and this will be set to a DiagnosticNegativeResponseCode returned
 *      by the other node.
 * payload - An optional payload for the response - NULL if no payload.
 * payload_length - The length of the payload or 0 if none.
 */
typedef struct
{
    bool completed;
    bool success;
    bool multi_frame;
    uint32_t arbitration_id;
    uint8_t mode;
    bool has_pid;
    uint16_t pid;
    DiagnosticNegativeResponseCode negative_response_code;
    uint8_t payload[MAX_UDS_RESPONSE_PAYLOAD_LENGTH];
    uint8_t payload_length;
} DiagnosticResponse;

#endif

