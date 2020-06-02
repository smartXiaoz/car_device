/*
******************************************************************
*
* 平台数据类型定义
*
******************************************************************
*/
#ifndef _PLATFORM_TYPES_H_
#define _PLATFORM_TYPES_H_

#include "hardware.h"

typedef unsigned char boolean;
typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
typedef signed char sint8;
typedef signed int sint16;
typedef signed long sint32;

#define  OFF          0
#define  ON           1

#define  STD_LOW      0
#define  STD_HIGH     1

#define STD_ON      1
#define STD_OFF     0


typedef uint8 Std_ReturnType;


#ifndef E_OK
#define E_OK        0x00 
#define E_NOT_OK    0x01
#endif
 

#ifndef TRUE
#define TRUE        1u           
#define FALSE       0u       
#endif


#ifndef NULL
#define NULL 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif /* #ifndef PLATFORM_TYPES_H */
