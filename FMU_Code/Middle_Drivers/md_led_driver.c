/*******************************************************************************
* �ļ����ƣ�md_led_driver.c
*
* ժ    Ҫ��STM32F1��LED�м�������ʵ�֣��ײ�Ӳ�����������Ӧ�ò�֮��Ĳ㣩
*
* ��ǰ�汾��
* ��    �ߣ�
* ��    �ڣ�2017/12/18
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/

#include "md_led_driver.h"


/*******************************��������****************************************
* ��������: void MD_LED_AMBER_Control(uint8_t status)
* �������: status�����ƵƵ�����״̬
* ���ز���:  
* ��    ��:
* ��    ��: 
* ��    ��: 2017/12/18
*******************************************************************************/ 
void MD_LED_AMBER_Control(uint8_t status)
{
	if(status==1)
	{
		//����
	  HAL_GPIO_WritePin(FMU_LED_AMBER_GPIO_Port,FMU_LED_AMBER_Pin,GPIO_PIN_RESET);
	}
	else
	{
		//Ϩ��
	   HAL_GPIO_WritePin(FMU_LED_AMBER_GPIO_Port,FMU_LED_AMBER_Pin,GPIO_PIN_SET);
	}
}




uint8_t count_led=0;

void Red_Fast_Flashing_Control(void)
{
	if(count_led%2 ==0)
	{
		static uint8_t falg_Red_Fast_Flashing=0;
		if(falg_Red_Fast_Flashing==1)
		{
			//�������
			MD_LED_AMBER_Control(LED_ON);
			falg_Red_Fast_Flashing=0;
		}
		else
		{
			//Ϩ����
			MD_LED_AMBER_Control(LED_OFF);
			falg_Red_Fast_Flashing=1;
		}
	}
}

void Red_Slow_Flashing_control(void)
{
	if(count_led%5 ==0)
	{
		static uint8_t falg_Red_Slow_Flashing=0;
		if(falg_Red_Slow_Flashing==1)
		{
			MD_LED_AMBER_Control(LED_ON);
			falg_Red_Slow_Flashing=0;
		}
		else
		{
			MD_LED_AMBER_Control(LED_OFF);
			falg_Red_Slow_Flashing=1;
		}
	}
}

void LED_Display(enum LED_STATE mRed_State)
{
	switch(mRed_State)
	{
		case Red_OFF_Light:
			MD_LED_AMBER_Control(LED_OFF);
		break;
		
		case Red_Fast_Flashing:
			Red_Fast_Flashing_Control();
		break;
		
		case Red_Slow_Flashing:
			Red_Slow_Flashing_control();
		break;
		
		case Red_ON_Lighting:
			MD_LED_AMBER_Control(LED_ON);
		break;
		
		default:
			MD_LED_AMBER_Control(LED_OFF);
		break;
	}
}	



enum LED_STATE mRed_State   = Red_OFF_Light;

void LED_Handler(void)
{
	count_led++;
	count_led = count_led >= 50 ? 0 : count_led;
	LED_Display(mRed_State);
}

void set_mRed_State(enum LED_STATE state)
{
	mRed_State = state;
}

