/*
******************************************************************
*
*
*
******************************************************************
*/
#ifndef  _UDS_TYPES_H_
#define  _UDS_TYPES_H_


#include "Platform_Types.h"


//Dsp
typedef struct
{
    uint8 SecurityLevel;
}SecurityRow,*SecurityRowRef;


typedef struct 
{
    uint8 SessionLevel;
}SessionRow,*SessionRowRef;


typedef struct 
{   
    const SessionRow *  Session;
}Dsp;


//Dsd
typedef struct 
{
    uint8   ServiceId;						//ID
    uint8   SubfuncAvial;  
    uint16  Subfunc;
    const   SecurityRow *SecurityLevelRef;
    const   SessionRow *SessionLevelRef;
}UdsService;

typedef struct 
{
    uint32   Id;
    const    UdsService *Services;
    uint8    ServiceNum;
}UdsServiceTable;


typedef struct 
{
    const UdsServiceTable *ServiceTables;
    uint8 ServiceTableNum;
}Dsd;

//总的定义
typedef struct 
{
    const Dsd _Dsd;
    const Dsp _Dsp;
}UdsConfig;



typedef struct 
{
    uint32    Address;
    uint32    Size;
    uint32    PageSize;
}FlashPage;

 

#endif  /*  _UDS_TYPES_H_  */
