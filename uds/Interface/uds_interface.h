/*
******************************************************************
*
* monli
*
******************************************************************
*/
#ifndef _UDS_INTERFACE_H_
#define _UDS_INTERFACE_H_

#include "Interface/uds_cfg.h"
#include "common/uds_types.h"


#define APP_VALID    0x00
#define APP_INVALID  0xF1

#define PROGRAMSESSION_VALID   0xAA
#define PROGRAMSESSION_INVALID 0xCC

uint8 Uds_DiagnosticSessionControlPreCheck(uint8 currentSession,uint8 targetSession);

void Uds_DiagnosticSessionControl(uint8 session);
void Uds_SwitchProgramSession(void);

void Uds_TesterPresent(void);

void Uds_ControlDTCSetting(uint8 DTCSettingType);

void Uds_CommunicationControl(uint8 controlType,uint8 communicationType);

uint8 Uds_EcuResetPreCheck(uint8 resetType);

void Uds_EcuReset(uint8 resetType);

uint8 Uds_RequestSeed(uint8 securityAccessType,uint8* securityAccessDataRecord, uint8* securitySeed,uint8* securitySeedSize);

uint8 Uds_CompareKey(uint8 securityAccessType,uint8* securitySeed, uint8* securityKey);

uint8 Uds_ReadDataByIdentifier(uint16 dataIdentifier,uint8* dataRecord,uint8* dataRecordSize);

uint8 Uds_WriteDataByIdentifier(uint16 dataIdentifier,uint8* dataRecord,uint8 dataRecordSize);

uint32 Uds_RoutineControl(uint8 subFunction,uint16 RoutineId, uint8* pRxBufPtr,uint8* pTxBufPtr,uint8 *routineStatusRecordSize);

uint8 Uds_InputOutputControlByIdentifier(uint16 dataIdentifier,uint8* controlOptionRecord,uint8* controlStatusRecord,uint16* controlStatusRecordSize);





uint8 Uds_ClearDTC(uint32 groupOfDTC);

uint8 Uds_ReportNumberOfDTCByStatusMask(uint8 DTCStatusMask,uint8* DTCs,uint16* resLength);

uint8 Uds_ReportDTCByStatusMask(uint8 DTCStatusMask,uint8* DTCs,uint16* resLength);

uint8 Uds_ReportMirrorMemoryDTCByStatusMask(uint8 DTCStatusMask,uint8* DTCs,uint16* resLength);

uint8 Uds_ReportNumberOfMirrorMemoryDTCByStatusMask(uint8 DTCStatusMask,uint8* DTCs,uint16* resLength);

uint8 Uds_ReportNumberOfEmissionsRelatedOBDDTCByStatusMask(uint8 DTCStatusMask,uint8* DTCs,uint16* resLength);

uint8 Uds_ReportEmissionsRelatedOBDDTCByStatusMask(uint8 DTCStatusMask,uint8* DTCs,uint16* resLength);

uint8 Uds_ReportDTCSnapshotIdentification (uint8* DTCSnapshotIdentification,uint16* resLength);

uint8 Uds_ReportDTCSnapshotRecordByDTCNumber (uint32 DTCMaskRecord,uint8 DTCSnapshotRecordNumber,uint8* DTCSnapshotRecord,uint16* resLength);

uint8 Uds_ReportDTCSnapshotRecordByRecordNumber(uint8 DTCSnapshotRecordNumber,uint8* DTCSnapshotRecord,uint16* resLength);

uint8 Uds_ReportDTCExtendedDataRecordByDTCNumber(uint32 DTCMaskRecord,uint8 DTCExtendedDataRecordNumber,uint8* DTCExtendedDataRecord,uint16* resLength);

uint8 Uds_ReportMirrorMemoryDTCExtendedDataRecordByDTCNumber(uint32 DTCMaskRecord,uint8 DTCExtendedDataRecordNumber,uint8* MirrorMemoryDTCExtendedDataRecord,uint16* resLength);

uint8 Uds_ReportNumberOfDTCBySeverityMaskRecord(uint32 DTCMaskRecord,uint8 DTCExtendedDataRecordNumber,uint8* NumberOfDTC,uint16* resLength);

uint8 Uds_ReportDTCBySeverityMaskRecord(uint32 DTCMaskRecord,uint8 DTCExtendedDataRecordNumber,uint8* DTCBySeverityMaskRecord,uint16* resLength);  
    
uint8 Uds_ReportSeverityInformationOfDTC(uint32 DTCMaskRecord,uint8* SeverityInformationOfDTC,uint16* resLength); 
              
uint8 Uds_ReportSupportedDTC(uint8* SupportedDTC,uint16* resLength);

uint8 Uds_ReportFirstTestFailedDTC(uint8* FirstTestFailedDTC,uint16* resLength);

uint8 Uds_ReportFirstConfirmedDTC(uint8* FirstConfirmedDTC,uint16* resLength);

uint8 Uds_ReportMostRecentTestFailedDTC(uint8* MostRecentTestFailedDTC,uint16* resLength);

uint8 Uds_ReportMostRecentConfirmedDTC(uint8* MostRecentConfirmedDTC,uint16* resLength);

uint8 Uds_ReportDTCFaultDetectionCounter(uint8* DTCFaultDetectionCounter,uint16* resLength);

uint8 Uds_ReportDTCWithPermanentStatus(uint8* DTCWithPermanentStatus,uint16* resLength);





uint32 Uds_RequestDownload(uint32 address,uint32 size);

uint32 Uds_TransferData(uint8 Sequnce,uint8* data,uint32 length);

uint32 Uds_RequestTransferExit(void);



uint32 Uds_GetTime(void);



void Uds_ClearAppFlag(void);

uint8 Uds_ReadAppFlag(void);

void Uds_WriteAppFlag(void);



void Uds_ClearProgramSessionFlag(void);

uint8 Uds_ReadProgramSessionFlag(void);

void Uds_WriteProgramSessionFlag(void);


extern const uint32 DTC[];
extern const FlashPage  FlashPages[];

#endif
