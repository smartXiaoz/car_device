#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


#define PWM1	TIM1->CCR1
#define PWM2	TIM1->CCR4
#define PWM3	TIM3->CCR1
#define PWM4	TIM3->CCR2
#define PWM5	TIM3->CCR3
#define PWM6	TIM3->CCR4

void TIM1_PWM_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);

#endif























