/*******************************************************************************
* �ļ����ƣ�example.c
*
* ժ    Ҫ�����Գ���
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/24
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#include "example.h"
#include "md_include.h"
#include "md_struct.h"

/*******************************��������****************************************
* ��������: void Example_RCin_PWMout_Direct(void)
* �������: void
* ���ز���: void
* ��    ��: ֱ�ӽ���ȡ��ң��������ֵ�����PWM�ڣ�����PWM�Ƿ���ȷ��
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void Example_RCin_PWMout_Direct(void)
{
	/*��ȡ����ͨ����ֵ 1094---1933*/
	
  uint16_t m_throttle = rc_pwm_in[2];
	PWM_Set_Value_Channel_1(m_throttle);
	PWM_Set_Value_Channel_2(m_throttle);
	PWM_Set_Value_Channel_3(m_throttle);
	PWM_Set_Value_Channel_4(m_throttle);
	PWM_Set_Value_Channel_5(m_throttle);
	PWM_Set_Value_Channel_6(m_throttle);
	PWM_Set_Value_Channel_7(m_throttle);
	PWM_Set_Value_Channel_8(m_throttle);
}


extern __IO ITStatus rc_newdata_flag;
extern uint16_t rc_pwm_in[RC_INPUT_CHANNELS];
extern Motor_Value m_Motor_From_F4;
void Example_Exchage_COM(void)
{	
	/*1.����ң�������ݸ�F4*/
	if(rc_newdata_flag==SET)
	{
	  rc_newdata_flag=RESET;
		Com_TwoBoard_Msg_RCinput_Send(rc_pwm_in);
		Com_TwoBoard_Msg_HeartBeat_Send(Status_Safety);
	}
	/*2.����F4���ص�PWM����*/
	Com_TwoBoard_RB_Update();
	/*3.���Ƶ��ת��*/
	PWM_Set_Value_Channel_1(m_Motor_From_F4.motor1);
	PWM_Set_Value_Channel_2(m_Motor_From_F4.motor2);
	PWM_Set_Value_Channel_3(m_Motor_From_F4.motor3);
	PWM_Set_Value_Channel_4(m_Motor_From_F4.motor4);
	
}
