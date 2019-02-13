#include "key.h"

uint8_t short_key_flag = 0; //短按键标志
uint8_t key_long_down = 0;  //长按键标志
uint8_t key_fall_flag = 0;  //按键按下标志

void KEY_Init(void)
{
	//初始化引脚 PA2
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //GPIOE时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // KEY 对应引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//50
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

	//初始化定时器TIM3
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); ///使能TIM3时钟

	TIM_TimeBaseInitStructure.TIM_Period = 100; 	//定时器溢出上限 //大概每ms溢出一次
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //初始化TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3, ENABLE); //使能定时器3

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//按键按下去会出现下降沿，设置按键IO口所在的外部端口为下降沿触发中断。
	
	//初始化中断
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	KEY_Init(); //①按键端口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	//GPIOE.2 中断线以及中断初始化配置,下降沿触发
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, GPIO_PinSource2); //
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //使能按键外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级 2，
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //子优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);
}

//⑥外部中断 2 服务程序  按键效果！！！
void EXTI2_IRQHandler(void)
{
	if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0) //按键 KEY2
	{
		key_fall_flag = 1;//生成按键按下标志
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //清除 LINE2 上的中断标志位
}


//中断效果
void TIM3_IRQHandler(void) //TIM3 中断
{
	uint16_t keyupCnt = 0;//按键弹起后计数值
	uint16_t key_holdon_ms = 0;//按下的时长
	bool keyUpFlag = TRUE;//按键弹起标志

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查 TIM3 更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除 TIM3 更新中断标志
		if (key_fall_flag == 1) //发生按键按下事件(按键中断发生,在按键中断中设置为1)
		{
			if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0) //按键还是按下的
			{
				if (key_holdon_ms <= 2000)
				{
					key_holdon_ms++;
				}
				else if (key_holdon_ms > 2000) //按键按下到2000ms就判断长按时间成立，生成长按标志
				{
					key_holdon_ms = 0;	//清除时间单位
					short_key_flag = 0; //清短按键标志
					key_long_down = 1;  //长按键标志置位！！！OK
					key_fall_flag = 0;  //清按键按下标志
					keyUpFlag = FALSE;  //标记按下
					keyupCnt = 0;
				}
				//距离上次单击时间在100~500ms之间，则认为发生连击事件
				if ((keyupCnt > 300) && (keyupCnt < 500))
				{
					keyupCnt = 0;
					doubleClick = 1;	//标记发生了连击事件！！OK
				}

			}
			else //当时是按下去的，此时已经释放按键了
			{
				keyupCnt = 0;
				if (key_holdon_ms > 50) //按下时间大于50ms，生成单击标志
				{
					key_holdon_ms = 0;
					short_key_flag = 1;	//标记短按标志！！OK
					key_long_down = 0;	//清除长按标志
					key_fall_flag = 0;
					keyupCnt = 0;

					//距离上次单击时间在100~500ms之间，则认为发生连击事件
					if (keyupCnt > 100 && keyupCnt < 500)
					{
						doubleClick = TRUE;
						short_key_flag = 0;
					}
					keyUpFlag = TRUE;//单击抬起按键后，生成按键抬起标志 
				}
				else  //按键持续时间小于50ms，忽略
				{
					key_holdon_ms = 0;//按键按下时间的位
					short_key_flag = 0;//短按
					key_long_down = 0;//长按标志
					key_fall_flag = 0;//按键按下标志
					keyupCnt = 0;
				}

			}


		}


		if (keyUpFlag)//单击抬起后，启动计数，计数到500ms
			keyupCnt++;


		if (keyupCnt > 500)
		{
			keyupCnt = 0;
			keyUpFlag = FALSE;//标记为弹起
		}


	}
}