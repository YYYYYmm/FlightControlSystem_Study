/*******************************************************************************
* �ļ����ƣ�md_pwm_output.h
*
* ժ    Ҫ��STM32F1��PWM�������
*
* ��ǰ�汾��
* ��    �ߣ�xiaodaqi	
* ��    �ڣ�2017/12/18
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/

#ifndef __MD_PWM_OUTPUT_H
#define __MD_PWM_OUTPUT_H

#include "main.h"
#include "stm32f1xx_hal.h"

#define PWM_MINI_OUTPTU_TO_DRIVER  1000   //���0ת������ʱ��1000us
#define PWM_MAX_OUTPTU_TO_DRIVER   2000   //������ת������ʱ��2000us

void PWM_Set_Output_Enable(void);
void PWM_Set_Value_Channel_1(uint16_t value);
void PWM_Set_Value_Channel_2(uint16_t value);
void PWM_Set_Value_Channel_3(uint16_t value);
void PWM_Set_Value_Channel_4(uint16_t value);
void PWM_Set_Value_Channel_5(uint16_t value);
void PWM_Set_Value_Channel_6(uint16_t value);
void PWM_Set_Value_Channel_7(uint16_t value);
void PWM_Set_Value_Channel_8(uint16_t value);

#endif
