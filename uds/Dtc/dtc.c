#include "Dtc/dtc.h"



union uds_dtc_status dtc_status[12];
//unsigned char dtcstatusconfirm[DTC_NUM];

static int DtcEnableState = 1;

static void SaveSnapshot(int index)
{
	
	
}

static void SaveEeprom(uint16 index)
{
	
	
	
}

/**
 * Enables dtc check.
 */
void uds_dtc_enable(void)
{
	DtcEnableState = 1;
}

/**
 * Disables dtc check.
 */
void uds_dtc_disable(void)
{
	DtcEnableState = 0;
}

/**
 * See if dtc check is enabled.
 *
 * @return 1 if enabled and 0 otherwise.
 */
int uds_dtc_is_enabled(void)
{
	return DtcEnableState;
}
/*
static u8 DTC_boDTCSet(int index)
{
	u8 boRet = FALSE;
	if(index <DTC_NUM ){
		if((dtc_status[index].bits.test_failed == TRUE )&& (dtc_status[index].bits.confirmed_dtc == TRUE))
			{
			boRet = TRUE;	
		}
	}

	return boRet;
}*/

static void DtcSort(int index)
{
	index = index;
}

static void SaveCurrentDtc(int index)
{
	if (dtc_status[index].bits.test_failed == 0)
	{
		dtc_status[index].bits.test_failed = 1;
		#if (test_not_completed_this_cycle_EN == ON )
		dtc_status[index].bits.test_not_completed_this_cycle = 1;
		#endif
		dtc_status[index].bits.confirmed_dtc = 1;
		SaveSnapshot(index);
		//DtcSort(index);
		SaveEeprom(index);
	}
}

static void SaveHistoryDtc(int index)//保存历史DTC
{
	if(dtc_status[index].bits.test_failed == 1)
	{
		dtc_status[index].bits.test_failed = 0;
		#if(test_not_completed_this_cycle_EN == ON)
		dtc_status[index].bits.test_not_completed_this_cycle = 1;
		#endif
		dtc_status[index].bits.confirmed_dtc = 1;
		SaveSnapshot(index);
		//DtcSort(index);
		SaveEeprom(index);
	}
}
/**
 * DTC check handler.
 */
extern void DTC_Happen(u8 num);
void uds_dtc_check(void)
{
	if (DtcEnableState == 0)
		return;

	if(PowerHigh)   //故障0 
	{
		SaveCurrentDtc(0);
		DTC_Happen(0);
	}
	else
	{
		SaveHistoryDtc(0);
	}

	if(PowerLow)   //故障0 
	{
		SaveCurrentDtc(1);
		DTC_Happen(1);
	}
	else
	{
		SaveHistoryDtc(1);
	}
	if(FAULT_Inf.BUSOFF_Flag){
		//dtc_status[4].bits.test_not_completed_since_last_clear &= (uint8)(~DTC_nenStatusTestNotCompletedSinceLastClear);
		SaveCurrentDtc(2);
		DTC_Happen(2);

	}else{
		SaveHistoryDtc(2);
	}
	if(FAULT_Inf.MISSBCM)   //故障  BCM接收超时
	{
		SaveCurrentDtc(3);
		DTC_Happen(3);
	}
	else
	{
		SaveHistoryDtc(3);
	}
	if(FAULT_Inf.MISSIC)   // IC 节点接收超时
	{
		SaveCurrentDtc(4);
		DTC_Happen(4);
	}
	else
	{
		SaveHistoryDtc(4);
	}
	if(FAULT_Inf.MISSABS)   //ABS 节点接收超时
	{
		SaveCurrentDtc(5);
		DTC_Happen(5);
	}
	else
	{
		SaveHistoryDtc(5);
	}
	if(FAULT_Inf.RADIO)    //收音机故障 不工作
	{
		SaveCurrentDtc(6);
	}
	else
	{
		SaveHistoryDtc(6);
	}
	if(FAULT_Inf.BLTH)    // 蓝牙不工作
	{
		SaveCurrentDtc(7);
	}
	else
	{
		SaveHistoryDtc(7);
	}
	if(FAULT_Inf.WIFI)   //WIFi不工作
	{
		SaveCurrentDtc(8);
	}
	else
	{
		SaveHistoryDtc(8);
	}
	if(FAULT_Inf.VOICE)   // 语音功能不工作
	{
		SaveCurrentDtc(9);
	}
	else
	{
		SaveHistoryDtc(9);
	}
	if(FAULT_Inf.LOA)    // 369环视不工作
	{
		SaveCurrentDtc(10);
	}
	else
	{
		SaveHistoryDtc(10);
	}
	if(FAULT_Inf.MPUDIED)    // 多媒体死机
	{
		SaveCurrentDtc(11);
	}
	else
	{
		SaveHistoryDtc(11);
	}
	//故障1
	//故障 n
	
}



//
void DTC_Clean(u8 num)
{
	SaveHistoryDtc(num);
	
}











