#ifndef _HDD_H
#define	_HDD_H

#include "stm32f10x.h"

//#define HDD_IN_PIN PA15

void HDD_EXTI_Init(void);
void EXTI15_10_IRQHandler(void);

#endif /* __LED_H */
