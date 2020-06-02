/*
 * uds_dtc_check.h
 *
 *  Created on: 2015Äê9ÔÂ11ÈÕ
 *      Author: XTD
 */

#ifndef UDS_DTC_CHECK_H_
#define UDS_DTC_CHECK_H_
#include "common/Platform_Types.h"
#include "Interface/uds_cfg.h"



#define test_not_completed_this_cycle_EN   0
typedef enum
{
    DTC_nenStatusTestFailed                     = 0x01u,   /* failed at this moment in time (mode=on) */
    DTC_nenStatusTestFailedThisCycle            = 0x02u,   /* failed this ign cycle */
    DTC_nenStatusPendingDtc                     = 0x04u,   /* possible problem in current or previous ign cycle */
    DTC_nenStatusConfirmedDtc                   = 0x08u,   /* definite problem */
    DTC_nenStatusTestNotCompletedSinceLastClear = 0x10u,   /* not checked since last clear */
    DTC_nenStatusTestFailedSinceLastClear       = 0x20u,   /* failed since last clear */
    DTC_nenStatusTestNotCompletedThisCycle      = 0x40u,   /* test not run on this ignition cycle */
    DTC_nenStatusWarningIndication              = 0x80u    /* a warning LED or popup was displayed with this fault */
} DTC_tenStatus;

/*
union uds_dtc_status {
	struct {
		uint8 warning_indicator_requested:1;
		uint8 test_not_completed_this_cycle:1;
		uint8 test_failed_since_last_clear:1;
		uint8 test_not_completed_since_last_clear:1;
		uint8 confirmed_dtc:1;
		uint8 pending_dtc:1;
		uint8 test_failed_this_cycle:1;
		uint8 test_failed:1;
	} bits;
	uint8 all;
};
*/
union uds_dtc_status {
	struct {
		uint8 test_failed:1;
		uint8 test_failed_this_cycle:1;
		uint8 pending_dtc:1;
		uint8 confirmed_dtc:1;
		uint8 test_not_completed_since_last_clear:1;
		uint8 test_failed_since_last_clear:1;
		uint8 test_not_completed_this_cycle:1;
		uint8 warning_indicator_requested:1;	
	} bits;
	uint8 all;
};

void uds_dtc_enable(void);
static u8 DTC_boDTCSet(int index);

void uds_dtc_disable(void);
int uds_dtc_is_enabled(void);

void uds_dtc_check(void);

#endif /* UDS_DTC_CHECK_H_ */
