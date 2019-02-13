
#ifndef _KEY_H
#define	_KEY_H

#include "stm32f10x.h"

//#define HDD_IN_PIN PA15

void HDD_EXTI_Init(void);
void KEY_Init(void);
void EXTI2_IRQHandler(void);
void TIM3_IRQHandler(void);

#endif /* __LED_H */