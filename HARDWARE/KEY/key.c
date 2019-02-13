#include "key.h"

uint8_t short_key_flag = 0; //�̰�����־
uint8_t key_long_down = 0;  //��������־
uint8_t key_fall_flag = 0;  //�������±�־

void KEY_Init(void)
{
	//��ʼ������ PA2
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //GPIOEʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // KEY ��Ӧ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//50
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4

	//��ʼ����ʱ��TIM3
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); ///ʹ��TIM3ʱ��

	TIM_TimeBaseInitStructure.TIM_Period = 100; 	//��ʱ��������� //���ÿms���һ��
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //��ʼ��TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3, ENABLE); //ʹ�ܶ�ʱ��3

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//��������ȥ������½��أ����ð���IO�����ڵ��ⲿ�˿�Ϊ�½��ش����жϡ�
	
	//��ʼ���ж�
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	KEY_Init(); //�ٰ����˿ڳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	//GPIOE.2 �ж����Լ��жϳ�ʼ������,�½��ش���
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource2); //
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //ʹ�ܰ����ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
}

//���ⲿ�ж� 2 �������  ����Ч��������
void EXTI2_IRQHandler(void)
{
	if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0) //���� KEY2
	{
		key_fall_flag = 1;//���ɰ������±�־
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //��� LINE2 �ϵ��жϱ�־λ
}


//�ж�Ч��
void TIM3_IRQHandler(void) //TIM3 �ж�
{
	uint16_t keyupCnt = 0;//������������ֵ
	uint16_t key_holdon_ms = 0;//���µ�ʱ��
	bool keyUpFlag = TRUE;//���������־

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //��� TIM3 �����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //��� TIM3 �����жϱ�־
		if (key_fall_flag == 1) //�������������¼�(�����жϷ���,�ڰ����ж�������Ϊ1)
		{
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0) //�������ǰ��µ�
			{
				if (key_holdon_ms <= 2000)
				{
					key_holdon_ms++;
				}
				else if (key_holdon_ms > 2000) //�������µ�2000ms���жϳ���ʱ����������ɳ�����־
				{
					key_holdon_ms = 0;	//���ʱ�䵥λ
					short_key_flag = 0; //��̰�����־
					key_long_down = 1;  //��������־��λ������OK
					key_fall_flag = 0;  //�尴�����±�־
					keyUpFlag = FALSE;  //��ǰ���
					keyupCnt = 0;
				}
				//�����ϴε���ʱ����100~500ms֮�䣬����Ϊ���������¼�
				if ((keyupCnt > 300) && (keyupCnt < 500))
				{
					keyupCnt = 0;
					doubleClick = 1;	//��Ƿ����������¼�����OK
				}

			}
			else //��ʱ�ǰ���ȥ�ģ���ʱ�Ѿ��ͷŰ�����
			{
				keyupCnt = 0;
				if (key_holdon_ms > 50) //����ʱ�����50ms�����ɵ�����־
				{
					key_holdon_ms = 0;
					short_key_flag = 1;	//��Ƕ̰���־����OK
					key_long_down = 0;	//���������־
					key_fall_flag = 0;
					keyupCnt = 0;

					//�����ϴε���ʱ����100~500ms֮�䣬����Ϊ���������¼�
					if (keyupCnt > 100 && keyupCnt < 500)
					{
						doubleClick = TRUE;
						short_key_flag = 0;
					}
					keyUpFlag = TRUE;//����̧�𰴼������ɰ���̧���־ 
				}
				else  //��������ʱ��С��50ms������
				{
					key_holdon_ms = 0;//��������ʱ���λ
					short_key_flag = 0;//�̰�
					key_long_down = 0;//������־
					key_fall_flag = 0;//�������±�־
					keyupCnt = 0;
				}

			}


		}


		if (keyUpFlag)//����̧�������������������500ms
			keyupCnt++;


		if (keyupCnt > 500)
		{
			keyupCnt = 0;
			keyUpFlag = FALSE;//���Ϊ����
		}


	}
}