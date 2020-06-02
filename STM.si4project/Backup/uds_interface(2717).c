/*
******************************************************************
*
*
******************************************************************
*/
#include "common/Platform_Types.h"
#include "Pdu/pdu.h"
#include "Interface/uds_interface.h"
#include "Dtc/dtc.h"
#include "hardware.h"

extern union uds_dtc_status dtc_status[];
int mask=0x3210;
int ComputeKey1(int seed)
{
	if (seed !=0)
	 { int i;
				for (i=0; i<17; i++ ) 
				 {
					if (seed&0x8000) 
					 {
							seed = seed <<1; 
							seed = seed^mask; 
					 }
					else if(seed==1)
					 {
							 seed = seed +2; 
					 }
					else
					 {
							 seed = seed +1;
					 }
				 }
		}
    return seed;
}

uint32 ComputeKey(uint8 securityAccessType, uint32 seed)
{
	/*if(securityAccessType == 1)
	{
		 return ComputeKey1(seed);
	}
	else*/ 
		if(securityAccessType == 3)
	{
		 return ComputeKey1(seed);
	}
}
void Uds_TesterPresent()
{

}

void Uds_SwitchProgramSession()
{
    uds.SessionLevel = 2;
}

void Uds_ControlDTCSetting(uint8 DTCSettingType)
{
	if (DTCSettingType == 0x01) // ON
		uds_dtc_enable();
	else if (DTCSettingType == 0x02) // OFF
		uds_dtc_disable();
}
void CommunicationType_Enable_Rx_Tx(communicationType)
{
	switch (communicationType)
	{
	case 0x01: // 甯歌搴旂敤鎶ユ枃
	//	NormalMessagesRxEnable();
	//	NormalMessagesTxEnable();
		break;

	case 0x02: // 缃戠粶绠＄悊鎶ユ枃
	//	NetworkManagementMessagesRxEnable();
	//	NetworkManagementMessagesTxEnable();
		break;

	case 0x03: //甯歌搴旂敤鎶ユ枃鍜岀綉缁滅鐞嗘姤鏂�
	//	NormalMessagesRxEnable();
	//	NormalMessagesTxEnable();
	//	NetworkManagementMessagesRxEnable();
	//	NetworkManagementMessagesTxEnable();
		break;

	}
}

void CommunicationType_Enable_Rx_Disable_Tx(communicationType)
{
	switch (communicationType)
	{
	case 0x01: // 甯歌搴旂敤鎶ユ枃
	//	NormalMessagesRxEnable();
	//	NormalMessagesTxDisable();
		break;

	case 0x02: // 缃戠粶绠＄悊鎶ユ枃
	//	NetworkManagementMessagesRxEnable();
	//	NetworkManagementMessagesTxDisable();
		break;

	case 0x03: //甯歌搴旂敤鎶ユ枃鍜岀綉缁滅鐞嗘姤鏂�
	//	NormalMessagesRxEnable();
	//	NormalMessagesTxDisable();
	//	NetworkManagementMessagesRxEnable();
	//	NetworkManagementMessagesTxDisable();
		break;

	}
}

void CommunicationType_Enable_Tx_Disable_Rx(communicationType)
{
	switch (communicationType)
	{
	case 0x01: // 甯歌搴旂敤鎶ユ枃
//		NormalMessagesRxDisable();
//		NormalMessagesTxEnable();
		break;

	case 0x02: // 缃戠粶绠＄悊鎶ユ枃
	//	NetworkManagementMessagesRxDisable();
	//	NetworkManagementMessagesTxEnable();
		break;

	case 0x03: //甯歌搴旂敤鎶ユ枃鍜岀綉缁滅鐞嗘姤鏂�
	//	NormalMessagesRxDisable();
	//	NormalMessagesTxEnable();
	//	NetworkManagementMessagesRxDisable();
	//	NetworkManagementMessagesTxEnable();
		break;

	}
}

void CommunicationType_Disable_Rx_Tx(communicationType)
{
	switch (communicationType)
	{
	case 0x01: // 甯歌搴旂敤鎶ユ枃
	//	NormalMessagesRxDisable();
	//	NormalMessagesTxDisable();
		break;

	case 0x02: // 缃戠粶绠＄悊鎶ユ枃
	//	NetworkManagementMessagesRxDisable();
	//	NetworkManagementMessagesTxDisable();
		break;

	case 0x03: //甯歌搴旂敤鎶ユ枃鍜岀綉缁滅鐞嗘姤鏂�
	//	NormalMessagesRxDisable();
	//	NormalMessagesTxDisable();
//		NetworkManagementMessagesRxDisable();
//		NetworkManagementMessagesTxDisable();
		break;

	}
}
void Uds_CommunicationControl(uint8 controlType, uint8 communicationType)
{
	switch (controlType)
	{
	case 0x00: // enable rx and tx
		CommunicationType_Enable_Rx_Tx(communicationType);
		break;

	case 0x01: // enable rx and disable tx
		CommunicationType_Enable_Rx_Disable_Tx(communicationType);
		break;

	case 0x02: // disable rx and enable tx
		CommunicationType_Enable_Tx_Disable_Rx(communicationType);
		break;

	case 0x03: // disable rx and tx
		CommunicationType_Disable_Rx_Tx(communicationType);
		break;
	}
}

uint8 Uds_EcuResetPreCheck(uint8 resetType)
{
		return E_OK; //Uds_E_CONDITIONSNOTCORRECT;
}

void Uds_EcuReset(uint8 resetType)
{
	if(uds.SessionLevel == 2)
	{
		
	}
}

uint8 Uds_RequestSeed(uint8  securityAccessType,
		              uint8 *securityAccessDataRecord,
		              uint8 *securitySeed,
		              uint8 *securitySeedSize)
{
    uint32 current = Uds_GetTime();

    if(securityAccessType == uds.SecurityLevel)
    {
        securitySeed[0] = 0x00;
        securitySeed[1] = 0x00;
        securitySeed[2] = 0x00;
        securitySeed[3] = 0x00;
        *securitySeedSize = 4;
        return E_OK;
    }
    //securitySeed[0] = current >> 24 & 0xFF;
   // securitySeed[1] = current >> 16 & 0xFF;
   // securitySeed[2] = current >> 8 & 0xFF;
  //  securitySeed[3] = current & 0xFF;
   // *securitySeedSize = 4;
   		securitySeed[0] = 0x01;
        securitySeed[1] = 0x02;
        securitySeed[2] = 0x03;
        securitySeed[3] = 0x04;
        *securitySeedSize = 4;
    return E_OK;
}

uint8 Uds_CompareKey(uint8 securityAccessType, uint8 *securitySeed, uint8 *securityKey)
{
    uint32 seed;
    uint32 key;
    uint32 validkey;

    seed= ((uint32)securitySeed[0]) << 24 |
    	  ((uint32)securitySeed[1] << 16) |
    	  ((uint32)securitySeed[2] << 8) |
    	  securitySeed[3];
    key = ((uint32)securityKey[0]) << 24 |
    	  ((uint32)securityKey[1] << 16) |
    	  ((uint32)securityKey[2] << 8) |
    	  securityKey[3];
    validkey = ComputeKey(securityAccessType, seed);
    if (key == validkey)
    {
        return E_OK;
    }
    return E_NOT_OK;
}


uint8 vin[17]; //read & write
uint8 Software_Reference_Number[10]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F183 only read
uint8 EcuPartNumber[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F187 {ASCLL,BCD,BCD,BCD,BCD,BCD}  only read
uint8 SystemSupplierID[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F18A {ASCLL,BCD,BCD,BCD,BCD,BCD}  only read
uint8 EcuHardwareVersionNumber[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F191  {ASCLL,ASCLL,BCD,ASCLL,BCD,BCD}  only read
uint8 EcuApplicationSoftwareVersionNumber[6]={0x53,0x56,0x01,0x2E,0x00,0x05};//F1A0  {ASCLL,ASCLL,BCD,ASCLL,BCD,BCD}  only read
uint8 EcuCalibrationSoftwareNumber[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F1A1  {ASCLL,ASCLL,BCD,ASCLL,BCD,BCD}  only read
uint8 EcuNetReferenceNumber[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F1A2  {ASCLL,ASCLL,BCD,ASCLL,BCD,BCD}  only read
uint8 ECUManufactureDate[3]={0xFF,0xFF,0xFF};//F18B  {BCD,BCD,BCD}  only read
uint8 SupplierECUSerialNumber[10]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F18C  {BCD,BCD,BCD}  only read
uint8 FingerprintDataIdentifier[14]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F184  {BCD,BCD,BCD}  only 10 02
uint8 CalibrationDataIdentifier[14]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//F1A3  {BCD,BCD,BCD}  read & write
uint8 GeneralConfiguration[4]={0xFF,0xFF,0xFF,0xFF};//read & write
/**
bit0   DDWS
bit1    BSD
bit2-7 保留

*/


uint8 Uds_ReadDataByIdentifier(uint16 dataIdentifier, uint8 *dataRecord, uint8 *dataRecordSize)
{
    uint8 i = 0;
    uint8 index = 0;
    uint8 temp = 0;
    uint16 tempresv = 0;
    switch (dataIdentifier)
    {
	case 0xF183:
		for (i = 0; i < 10; i++)
		{
			dataRecord[index++] = Software_Reference_Number[i];
		}
		*dataRecordSize = index;
		break;
	
	case 0xF190:
		for (i = 0; i < 17; i++)
		{
			vin[i] = TEST_DID_F190[i];
			dataRecord[index++] = vin[i];
		}
		
		*dataRecordSize = index;
		break;
	case 0xF187:
		for (i = 0; i <6 ; i++)
		{
			dataRecord[index++] = EcuPartNumber[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF18A:
		for (i = 0; i < 6; i++)
		{
			dataRecord[index++] = SystemSupplierID[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF191:
		for (i = 0; i < 6; i++)
		{
			dataRecord[index++] = EcuHardwareVersionNumber[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF1A0:
		for (i = 0; i < 6; i++)
		{
			dataRecord[index++] = EcuApplicationSoftwareVersionNumber[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF1A1:
		for (i = 0; i < 6; i++)
		{
			dataRecord[index++] = EcuCalibrationSoftwareNumber[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF1A2:
		for (i = 0; i < 6; i++)
		{
			dataRecord[index++] = EcuNetReferenceNumber[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF18B:
		for (i = 0; i < 3; i++)
		{
			dataRecord[index++] = ECUManufactureDate[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF18C:
		for (i = 0; i < 10; i++)
		{
			dataRecord[index++] = SupplierECUSerialNumber[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF184:
		for (i = 0; i < 14; i++)
		{
			dataRecord[index++] = FingerprintDataIdentifier[i];
		}
		*dataRecordSize = index;
		break;
	case 0xF1A3:
		for (i = 0; i < 14; i++)
		{
			dataRecord[index++] = CalibrationDataIdentifier[i];
		}
		*dataRecordSize = index;
		break;
	case 0x3700:
		for (i = 0; i < 4; i++)
		{
			dataRecord[index++] = GeneralConfiguration[i];
		}
		*dataRecordSize = index;
		break;

	default:
		return Uds_E_REQUESTOUTOFRANGE;
    }

    return E_OK;
}

uint8 Uds_WriteDataByIdentifier(uint16 dataIdentifier, uint8 *dataRecord, uint8 dataRecordSize)
{
    uint8 i = 0;
    uint8 index = 0;
    uint16 temp;
    switch (dataIdentifier)
    {
	case 0xF190:
		if(dataRecordSize != 17)
		{
			return Uds_E_incorrectMessageLengthOrInvalidFormat;
		}
		for (i = 0; i < 17; i++)
		{
			vin[i] = dataRecord[index++];	
		}
		
	case 0xF1A3:
		if(dataRecordSize != 14)
		{
			return Uds_E_incorrectMessageLengthOrInvalidFormat;
		}
		for (i = 0; i < 14; i++)
		{
			CalibrationDataIdentifier[i] = dataRecord[index++];
		}
	case 0x3700:
		if(dataRecordSize != 4)
		{
			return Uds_E_incorrectMessageLengthOrInvalidFormat;
		}
		for (i = 0; i < 4; i++)
		{
			GeneralConfiguration[i] = dataRecord[index++];
		}
	//保存在EEPROM
		
		break;
	default:
		return Uds_E_REQUESTOUTOFRANGE;
    }

    return E_OK;
}

uint8 Uds_InputOutputControlByIdentifier(uint16  dataIdentifier,
		                                 uint8  *controlOptionRecord,
		                                 uint8  *controlStatusRecord,
		                                 uint16 *controlStatusRecordSize)
{
	controlStatusRecord[0] = 0x00;
    *controlStatusRecordSize = 0;
    switch (dataIdentifier)
    {
	case 0xF082:
		break;

	default:
		return Uds_E_REQUESTOUTOFRANGE;
    }
    return E_OK;
}

uint8 Uds_ClearDTC(uint32 groupOfDTC)
{
	uint8 i;

	for (i = 0; i < DTC_NUM; i++)
	{
		dtc_status[i].all = 0;
	}


	//保存在EEPROM中API

    return E_OK;
}

uint8 Uds_ReadDtcStatus(uint32 index, uint32 DTCMaskRecord, uint8 DTCStatusMask)
{
	return dtc_status[index].all & 0x00ff;
}

uint8 Uds_ReportNumberOfDTCByStatusMask(uint8 DTCStatusMask, uint8 *DTCs, uint16 *resLength)
{
    uint16 i;
    uint16 count = 0;
    uint8 status;
    uint32 dtc;

    for (i = 0; i < DTC_NUM; i++)
    {
        dtc = DTC[i];
        status = Uds_ReadDtcStatus(i, dtc, DTCStatusMask);
        if (status)
        {
            count++;
        }
    }
    DTCs[0] = (count>>8) & 0x00ff;
    DTCs[1] = count & 0x00ff;
    *resLength = 2;
    return E_OK;
}

uint8 Uds_ReportDTCByStatusMask(uint8 DTCStatusMask, uint8 *DTCs, uint16 *resLength)
{
    uint16 i;
    uint16 count = 0;
    uint8 status;
    uint32 dtc;

    for (i = 0; i < DTC_NUM; i++)
    {
        dtc = DTC[i];
        status = Uds_ReadDtcStatus(i, dtc, DTCStatusMask);
        //if (DTCStatusMask == status)
        if (0 != status)
        {
            DTCs[count*4+0] = (dtc >> 16) & 0xFF;
            DTCs[count*4+1] = (dtc >> 8) & 0xFF;
            DTCs[count*4+2] = (dtc >> 0) & 0xFF;
            DTCs[count*4+3] = status;
            count++;
        }
    }
    *resLength = count*4;
    return E_OK;
}

uint8 Uds_ReportMirrorMemoryDTCByStatusMask(uint8 DTCStatusMask, uint8 *DTCs,uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportNumberOfMirrorMemoryDTCByStatusMask(uint8 DTCStatusMask, uint8 *DTCs,uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportNumberOfEmissionsRelatedOBDDTCByStatusMask(uint8 DTCStatusMask, uint8 *DTCs,uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportEmissionsRelatedOBDDTCByStatusMask(uint8 DTCStatusMask, uint8 *DTCs,uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportDTCSnapshotIdentification(uint8 *DTCSnapshotIdentification, uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportDTCSnapshotRecordByDTCNumber(uint32  DTCMaskRecord,
		                                     uint8   DTCSnapshotRecordNumber,
		                                     uint8  *DTCSnapshotRecord,
		                                     uint16 *resLength)
{
    uint8 i, len, index, dataIndex, status;

    for (i = 0; i < DTC_NUM; i++)
	{
    	if (DTCMaskRecord == DTC[i])
		{
			break;
		}
	}
    if(i == DTC_NUM)
    {
    	return Uds_E_REQUESTOUTOFRANGE;
    }
    status = Uds_ReadDtcStatus(i, DTCMaskRecord, 0x49);

    index = 0;
    dataIndex = 0;

    DTCSnapshotRecord[index++]= (DTCMaskRecord >> 16) & 0xFF;
    DTCSnapshotRecord[index++]= (DTCMaskRecord >> 8) & 0xFF;
    DTCSnapshotRecord[index++]= (DTCMaskRecord ) & 0xFF;
    DTCSnapshotRecord[index++]= status;

    if(status != 0)
    {
		DTCSnapshotRecord[index++]= 0x01;
		DTCSnapshotRecord[index++]= 0x04;

		DTCSnapshotRecord[index++]= 0x13;
		DTCSnapshotRecord[index++]= 0x01;
		DTCSnapshotRecord[index++]= 0;
		DTCSnapshotRecord[index++]= 0;
		DTCSnapshotRecord[index++]= 0;
		DTCSnapshotRecord[index++]= 0;

		DTCSnapshotRecord[index++]= 0x13;
		DTCSnapshotRecord[index++]= 0x02;
		DTCSnapshotRecord[index++]= 0;
		DTCSnapshotRecord[index++]= 0;
		


    }
    *resLength = index;

    return E_OK;
}

uint8 Uds_ReportDTCSnapshotRecordByRecordNumber(uint8   DTCSnapshotRecordNumber,
		                                        uint8  *DTCSnapshotRecord,
		                                        uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportDTCExtendedDataRecordByDTCNumber(uint32  DTCMaskRecord,
		                                         uint8   DTCExtendedDataRecordNumber,
		                                         uint8  *DTCExtendedDataRecord,
		                                         uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportMirrorMemoryDTCExtendedDataRecordByDTCNumber(uint32  DTCMaskRecord,
		                                                     uint8   DTCExtendedDataRecordNumber,
		                                                     uint8  *MirrorMemoryDTCExtendedDataRecord,
		                                                     uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportNumberOfDTCBySeverityMaskRecord(uint32  DTCMaskRecord,
		                                        uint8   DTCExtendedDataRecordNumber,
		                                        uint8  *numberOfDTC,
		                                        uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportDTCBySeverityMaskRecord(uint32  DTCMaskRecord,
		                                uint8   DTCExtendedDataRecordNumber,
		                                uint8  *DTCBySeverityMaskRecord,
		                                uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportSeverityInformationOfDTC(uint32  DTCMaskRecord,
		                                 uint8  *SeverityInformationOfDTC,
		                                 uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportSupportedDTC(uint8 *DTCs,uint16 *resLength)
{
    uint16 i;
    uint16 count = 0;
    uint8 status;
    uint32 dtc;

    for (i = 0; i < DTC_NUM; i++)
    {
        dtc = DTC[i];
        status = Uds_ReadDtcStatus(i, dtc, 0);
       // if (0 == status)
        {
            DTCs[count*4+0] = (dtc >> 16) & 0xFF;
            DTCs[count*4+1] = (dtc >> 8) & 0xFF;
            DTCs[count*4+2] = (dtc >> 0) & 0xFF;
            DTCs[count*4+3] = status;
            count++;
        }
    }
    *resLength = count*4;
    return E_OK;
}

uint8 Uds_ReportFirstTestFailedDTC(uint8 *FirstTestFailedDTC, uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportFirstConfirmedDTC(uint8 *FirstConfirmedDTC, uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportMostRecentTestFailedDTC(uint8 *MostRecentTestFailedDTC, uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportMostRecentConfirmedDTC(uint8 *MostRecentConfirmedDTC, uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportDTCFaultDetectionCounter(uint8 *DTCFaultDetectionCounter, uint16 *resLength)
{
    return E_OK;
}

uint8 Uds_ReportDTCWithPermanentStatus(uint8 *DTCWithPermanentStatus, uint16 *resLength)
{
    return E_OK;
}

uint32 Uds_GetTime()
{
    return uds.Tick;
}

#if UDS_BOOTLOADER == STD_OFF

uint8 Uds_DiagnosticSessionControlPreCheck(uint8 currentSession,uint8 targetSession)
{
	if(targetSession == 1)
	{
		CommunicationType_Enable_Rx_Tx(0x01);
	}
	if((currentSession == 2)&&(targetSession == 3))
	{
		return 0x7E;
	}
	if((currentSession == 1)&&(targetSession == 2))
	{
		return 0x7E;
	}
	if (targetSession == 2)
	{
		if(1)
		{
			return E_OK;
		}
		else
		{
			return 0x7E;
		}
	}

    return E_OK;
}

void Uds_DiagnosticSessionControl(uint8 session)
{
	uds.SecurityLevel = 0;
    if (session == 2)
    {

    }
}

uint32 Uds_RoutineControl(uint8  subFunction,
		                  uint16 RoutineId,
		                  uint8 *pRxBufPtr,
		                  uint8 *pTxBufPtr,
		                  uint8 *routineStatusRecordSize)
{
	if(RoutineId == 0xDF01)
	{
		pTxBufPtr[0] = 0;
		*routineStatusRecordSize = 1;
	}
	if(RoutineId == 0xDF02)
	{
		pTxBufPtr[0] = 0;
		*routineStatusRecordSize = 1;
	}
	return 0;
}

#endif

uint8 Uds_ReadAppFlag(void)
{
    return APP_INVALID;
}

void Uds_WriteAppFlag()
{

}

void Uds_ClearAppFlag(void)
{

}

uint8 Uds_ReadProgramSessionFlag(void)
{
    return PROGRAMSESSION_INVALID;
}

void Uds_WriteProgramSessionFlag()
{
}

void Uds_ClearProgramSessionFlag(void)
{

}
