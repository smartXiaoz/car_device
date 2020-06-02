/*
******************************************************************
*
* 
*
******************************************************************
*/

#include "Interface/uds_cfg.h"
#include "common/uds_types.h"
 

const SessionRow  Uds_DspSessionRow123[] =
{ 
    {
        0x01,    //defaultSession
    },
    {
        0x02,    //ProgramSession
    },
    {
        0x03,   //ExtensionSession
    },
    {
        0,      /*SessionLevel*/
    },
};

const SessionRow  Uds_DspSessionRow23[] = 
{ 
    {
        0x02,    /*SessionLevel*/
    },
    {
        0x03,    /*SessionLevel*/
    },
    {
        0,      /*SessionLevel*/
    },
};

const SessionRow  Uds_DspSessionRow12[] = 
{ 
    {
        0x01,    /*SessionLevel*/
    },
    {
        0x02,    /*SessionLevel*/
    },
    {
        0,      /*SessionLevel*/
    },
};

const SessionRow  Uds_DspSessionRow13[] = 
{ 
    {
        0x01,    /*SessionLevel*/
    },
    {
        0x03,    /*SessionLevel*/
    },
    {
        0,      /*SessionLevel*/
    },
};

const SessionRow  Uds_DspSessionRow2[] = 
{ 
    {
        0x02,    /*SessionLevel*/
    },
    {
        0,      /*SessionLevel*/
    },
};


const SessionRow  Uds_DspSessionRow3[] = 
{ 
    {
        0x03,    /*SessionLevel*/
    },
    {
        0,      /*SessionLevel*/
    },
};


const SecurityRow  Uds_DspSecurityRow01[]=
{ 
    {
        0x1        /*SecurityLevel */
    },
    {
        0,         /*SecurityLevel */
    },
};


const SecurityRow  Uds_DspSecurityRow03[]=
{ 
    {
        0x3        /*SecurityLevel */
    },
    {
        0,         /*SecurityLevel */
    },
};

 
const UdsService PAServiceTable[] = 
{ 
    {
        0x10,                        
        TRUE,                     
        NULL, 
        NULL,                     
        Uds_DspSessionRow13        
    },
    {
        0x11,                      
        TRUE,                      
        NULL,
        NULL,     
        Uds_DspSessionRow13        
    },
    {
        0x27,                    
        TRUE,         
        NULL,
        NULL,            
        Uds_DspSessionRow3
    },
    {
        0x28,
        TRUE,
        NULL,
        NULL,
        Uds_DspSessionRow13
    },
    {
        0x3e,                    
        TRUE,                        
        NULL,  
        NULL,                      
        Uds_DspSessionRow13         
    },
    {
        0x85,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow13
    },
    {
        0x22,                    
        TRUE,         
        NULL,   
        NULL,           
        Uds_DspSessionRow13   
    },
	{
		0x2e,
		TRUE,
		0x0109,
		Uds_DspSecurityRow03,
		Uds_DspSessionRow3
	},
    {
        0x14,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow13
    },
    {
        0x19,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow13
    },
    {
        0x2F,
        TRUE,
        NULL,  
        Uds_DspSecurityRow03,
        Uds_DspSessionRow3
    },  
    {
        0x31,               
        TRUE,   
        0x0202,               
        Uds_DspSecurityRow03                                                                                                                                                                                                                                                                                                                                           , /*SecurityLevelRef*/
        Uds_DspSessionRow23         
    },
    {
        0x31,               
        TRUE,   
        0x0203,               
        NULL                                                                                                                                                                                                                                                                                                                                           , /*SecurityLevelRef*/
        Uds_DspSessionRow23         
    },
    {
        0x31,               
        TRUE,   
        0xFF00,               
        Uds_DspSecurityRow03                                                                                                                                                                                                                                                                                                                                           , /*SecurityLevelRef*/
        Uds_DspSessionRow23         
    },    
    {
        0x31,               
        TRUE,   
        0xFF01,               
        Uds_DspSecurityRow03                                                                                                                                                                                                                                                                                                                                           , /*SecurityLevelRef*/
        Uds_DspSessionRow23         
    },    
    {
        0x31,               
        TRUE,   
        0x060E,               
        Uds_DspSecurityRow01                                                                                                                                                                                                                                                                                                                                           , /*SecurityLevelRef*/
        Uds_DspSessionRow23         
    },        
    {
        0x34,                 
        TRUE,    
        NULL,                
        Uds_DspSecurityRow03                                                                                                                                                                                                                                                                                                                                           , /*SecurityLevelRef*/
        Uds_DspSessionRow2        
    },
    {
        0x36,                 
        TRUE,      
        NULL,            
        Uds_DspSecurityRow03,  
        Uds_DspSessionRow2       
    },
    {
        0x37,               
        TRUE,   
        NULL,             
        Uds_DspSecurityRow03,                
        Uds_DspSessionRow2        
    }
};


const UdsService FAServiceTable[] = 
{ 
    {
        0x10,                        
        TRUE,                     
        NULL, 
        NULL,                       
        Uds_DspSessionRow123        
    },
    {
        0x11,                      
        TRUE,                      
        NULL, 
        NULL,        
        Uds_DspSessionRow23        
    },
    {
        0x28,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow3
    },
    {
        0x3e,                    
        TRUE,                        
        NULL,      
        NULL,                    
        Uds_DspSessionRow123         
    },
    {
        0x85,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow3
    },
    {
        0x22,                    
        TRUE,         
        NULL,  
        NULL,            
        Uds_DspSessionRow123   
    },
    {
        0x14,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow3
    },
    {
        0x19,
        TRUE,
        NULL,
        NULL,  
        Uds_DspSessionRow3
    }
};

//两个结构体是大的ID划分，物理ID和功能ID
//每个ID内部还有细的划分，分别是ID服务码,...等5个参数
const UdsServiceTable gDsdStable[] = 
{ 
    {
        PHYSICAL_ADDRESS,        /*Id*/
        PAServiceTable,          /*Services*/
        21                       /*ServiceNum*/
    },
    {                                               
        FUNCTIONAL_ADDRESS,      /*Id*/
        FAServiceTable,          /*Services*/
        8                        /*ServiceNum*/
    }
};
                         

const UdsConfig UDS =
{ 
    {
        gDsdStable,             /*_ptrUdsServiceTable*/
        2                       /*ServiceTableNum*/
    }, 
    {
        Uds_DspSessionRow123    /*Uds_SESSION_LEVEL_TABLE*/
    }
};


const uint32 DTC[DTC_NUM] =
 {
    0xD56217,   // 0 
    0xD56316,   // 1
    //0xC16987,   // 2 
    //0xC12787,   // 3 
    0xC02888,   // 2 
    0xC14087,   // 3 
    0xC15587,   // 4
    0xC12187,   // 5
    0xD26101,   // 6 
    0xD26201,   // 7 
    0xD26301,   // 8 
    0xD26401,   // 9 
    0xD26501,   // 10 
    0xD26601,   // 11 
   // 0xA01212,   // 14 
   // 0xA01312,   // 15 
   // 0xA01617,   // 16 
   // 0xA03517,   // 17 
   // 0xA03612,   // 18
 };





