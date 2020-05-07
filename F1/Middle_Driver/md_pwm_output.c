/*******************************************************************************
* �ļ����ƣ�md_pwm_output.c
*
* ժ    Ҫ��STM32F1��PWM�������
*
* ��ǰ�汾��
* ��    �ߣ�Acorss	
* ��    �ڣ�2018/05/23
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/

#include "md_pwm_output.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/*******************************��������****************************************
* ��������: void PWM_Set_Output_Enable(void)
* �������: 
* ���ز���:  
* ��    ��: ����PWM���ʹ��
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Output_Enable(void)
{
	//��ʼ�����������Ϊ10��
	PWM_Set_Value_Channel_1(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_2(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_3(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_4(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_5(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_6(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_7(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_8(PWM_MINI_OUTPTU_TO_DRIVER);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
}

/*******************************��������****************************************
* ��������: void PWM_Set_Output_Enable(void)
* �������: 
* ���ز���:  
* ��    ��: ����PWM���ʹ��
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Output_Disable(void)
{
	//��ʼ�����������Ϊ10��
	PWM_Set_Value_Channel_1(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_2(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_3(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_4(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_5(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_6(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_7(PWM_MINI_OUTPTU_TO_DRIVER);
	PWM_Set_Value_Channel_8(PWM_MINI_OUTPTU_TO_DRIVER);
  HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_4);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_1(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��1�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_1(uint16_t value)
{
	htim2.Instance->CCR1 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_2(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��2�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_2(uint16_t value)
{
  htim2.Instance->CCR2 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_3(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��3�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_3(uint16_t value)
{
  htim4.Instance->CCR3 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_4(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��4�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_4(uint16_t value)
{
    htim4.Instance->CCR4 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_5(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��5�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_5(uint16_t value)
{
   htim3.Instance->CCR1 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_6(uint16_t value)
* �������:value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��6�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_6(uint16_t value)
{
   htim3.Instance->CCR2 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_7(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��7�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_7(uint16_t value)
{
   htim3.Instance->CCR3 = value;
}

/*******************************��������****************************************
* ��������: void PWM_Set_Value_Channel_8(uint16_t value)
* �������: value����С�����ֵΪ0--2499��ʵ�ʵĵ������������ЧֵΪ1094--1933��
* ���ز���:  
* ��    ��: ����ͨ��8�����
* ��    ��: by Acorss
* ��    ��: 2018/05/23
*******************************************************************************/ 
void PWM_Set_Value_Channel_8(uint16_t value)
{
   htim3.Instance->CCR4 = value;
}
