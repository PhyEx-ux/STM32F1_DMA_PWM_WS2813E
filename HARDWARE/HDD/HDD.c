#include "HDD.h"

uint8_t HDD_Path_Flag = 0; //Ӳ�̵Ʋ�����ʶ��

void HDD_EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //���ù���ʱ�Ӵ�
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //�ر�JTAG �ز����������ν����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //ʹ��PA15����Ϊ������������ ����ʵ��û���Թ���������ܲ�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15); //����GPIOΪ�ж�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//�жϷ�����
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line15) != RESET)  //����Ƿ񴥷���Ӧ���ж�
	{
		HDD_Path_Flag = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line15);//����жϱ�־

}
