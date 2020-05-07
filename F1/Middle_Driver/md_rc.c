/*******************************************************************************
* �ļ����ƣ�md_rc.h
*
* ժ    Ҫ��ң�������C�ļ�
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/15
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#include "md_rc.h"
#include "md_rc_ppm.h"
#include "md_rc_sbus.h"
#include "md_config.h"



uint16_t rc_pwm_in[RC_INPUT_CHANNELS];
__IO ITStatus rc_newdata_flag   = RESET;
void RC_Input_Init(void)
{
	/*����ң�����Ĳ�ͬ���г�ʼ��*/
#ifdef RC_INPUT_PPM
	PPM_Init();
#else
	SBUS_Init();
#endif
}

void RC_Input_Loop(void)
{
	/*����ң�����Ĳ�ͬ��ȡ����*/
#ifdef RC_INPUT_PPM
	PPM_GetData(rc_pwm_in,RC_INPUT_CHANNELS);
#else
	SBUS_GetData(rc_pwm_in,RC_INPUT_CHANNELS);
#endif
}
