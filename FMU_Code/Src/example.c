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
#include "mc_att_control.h"

extern __IO ITStatus f_NewRCDataReady;
extern RC_Channel m_RC_input_from_F1;
extern d_Motor *pMotor;
extern d_RC *pRC_Normalization;

uint16_t temp = 0;

void Example_Control(RC_Channel*rc_input)
{
	temp = (uint16_t)((pRC_Normalization->Throttle + 1.0f)*500.0f) + MOTOR_MIN_POINT;//�ٷ�֮30-80
  temp = temp > 2000 ? 2000 : temp;
  temp = temp < MOTOR_MIN_POINT ? MOTOR_MIN_POINT : temp;
	
  pMotor->M1 = temp;
	pMotor->M2 = temp;
	pMotor->M3 = temp;
	pMotor->M4 = temp;
	pMotor->M5 = temp;
	pMotor->M6 = temp;
}

void Example_Exchage_COM(void)
{	
  /*1.��ȡң������ͨ��ֵ*/
	  Com_TwoBoard_RB_Update();
	/*2.���п�������*/
	if(f_NewRCDataReady==SET)
	{
	  f_NewRCDataReady = RESET;
		Example_Control(&m_RC_input_from_F1);
	  /*3.�������Ŀ���ֵ��F1*/
	 Com_TwoBoard_Msg_MotorValue_Send(pMotor);
	}
	 
}
