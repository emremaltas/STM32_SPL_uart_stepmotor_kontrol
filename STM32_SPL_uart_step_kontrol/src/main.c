/*
 * EMRE MALTAÞ 18.02.2022 sol->sola, sag->saga donus ve dur-> motoru durdurma
 */

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include<stdio.h>

GPIO_InitTypeDef GPIO_InitStruct;
USART_InitTypeDef USART3_InitStruct;
NVIC_InitTypeDef NVIC_InitStruct;

uint32_t sure=0;
char durum[8],donen_yon[20];
uint8_t adet=0,secili_yon=2;

void SysTick_Handler()
{
	if(sure>0)
		sure--;
}

void delay_ms(uint32_t bekle)
{
	sure = bekle;
	while(sure);
}

void GPIO_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 |
			GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD,&GPIO_InitStruct);

	SysTick_Config(SystemCoreClock/1000);
}

void USART3_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	USART3_InitStruct.USART_BaudRate = 9600;
	USART3_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART3_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART3_InitStruct.USART_Parity = USART_Parity_No;
	USART3_InitStruct.USART_StopBits = USART_StopBits_1;
	USART3_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART3,&USART3_InitStruct);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART3,ENABLE);
}

void NVIC_Config()
{
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);
}


void USART_Puts(USART_TypeDef* USARTx,volatile char *s)
{
	while(*s)
	{
		while(!(USARTx->SR & (0X00000040)));
		USART_SendData(USARTx,*s);
		*s++;
	}
}

void donus_secimi()
{

	if(durum[0] == 's' && durum[1] == 'a' && durum[2] == 'g' && adet<4 )
	{
		secili_yon = 1; sprintf(donen_yon,"Saða dönülüyor!!\n"); USART_Puts(USART3,donen_yon);
	}
	else if(durum[0] == 's' && durum[1] == 'o' && durum[2] == 'l' && adet<4)
	{
		secili_yon = 0; sprintf(donen_yon,"Sola dönülüyor!!\n"); USART_Puts(USART3,donen_yon);
	}
	else if(durum[0] == 'd' && durum[1] == 'u' && durum[2] == 'r' && adet<4)
	{
		secili_yon = 2; sprintf(donen_yon,"Durduruldu!!\n"); USART_Puts(USART3,donen_yon);
	}
}

void USART3_IRQHandler()
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET )
	{
		char harf = USART_ReceiveData(USART3);

		if(adet<4 && harf != '\n')
		{
			durum[adet] = harf;
			adet++;
		}
		else
		{
			donus_secimi();
			adet=0;
			for(uint8_t t=0;t<4;t++)
			{
				durum[t] = '\0';
			}
		}
	}

	USART_ClearITPendingBit(USART3,USART_IT_RXNE);
}


int main(void)
{
	GPIO_Config();
	NVIC_Config();
	USART3_Config();

	while (1)
	{
		if(secili_yon == 0)
		{
			while(secili_yon == 0) //SOL
			{

				GPIO_SetBits(GPIOD,0x00008001 );
				GPIO_ResetBits(GPIOD,0x0000700E);
				delay_ms(50);

				GPIO_SetBits(GPIOD,0X00004002);
				GPIO_ResetBits(GPIOD,0X0000B00D);
				delay_ms(50);

				GPIO_SetBits(GPIOD,0X00002004 );
				GPIO_ResetBits(GPIOD,0X0000D00B);
				delay_ms(50);

				GPIO_SetBits(GPIOD,0X00001008 );
				GPIO_ResetBits(GPIOD,0X0000E007);
				delay_ms(50);
			}
		}

		else if(secili_yon == 1)
		{
			while(secili_yon == 1) //SAG
			{

				GPIO_SetBits(GPIOD,0X00001008);
				GPIO_ResetBits(GPIOD,0X0000E007);
				delay_ms(50);

				GPIO_SetBits(GPIOD,0X00002004);
				GPIO_ResetBits(GPIOD,0X0000D00B);
				delay_ms(50);

				GPIO_SetBits(GPIOD,0X00004002 );
				GPIO_ResetBits(GPIOD,0X0000B00D);
				delay_ms(50);

				GPIO_SetBits(GPIOD,0X00008001 );
				GPIO_ResetBits(GPIOD,0X0000700E);
				delay_ms(50);
			}
		}
		else if(secili_yon == 2) //DURDUR
		{
			while(secili_yon ==2)
			{
				GPIO_ResetBits(GPIOD,0x0000F00F);
			}
		}

	}
}


void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){

	return;
}


uint16_t EVAL_AUDIO_GetSampleCallBack(void){

	return -1;
}
