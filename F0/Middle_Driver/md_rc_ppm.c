/*******************************************************************************
* �ļ����ƣ�md_rc_ppm.c
*
* ժ    Ҫ��ң����PPM�������C�ļ�
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/15
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#include "md_rc_ppm.h"
#include <string.h>
#include "tim.h"
#include "string.h"

//��¼�ϴβ�׽���ļ�������ֵ
__IO uint16_t ppm_last_count;

//����ң������ͨ��ֽ
__IO uint16_t ppm_buffer[PPM_MAX_CHANNELS];
__IO ITStatus ppm_frame_captured = RESET;
__IO uint16_t ppm_count_temp=0;
/*******************************��������****************************************
* ��������: void PPM_Init(void)
* �������: void
* ���ز���: void
* ��    ��: PPM�����ʼ��������ʹ���ж�
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void PPM_Init(void)
{
	memset((uint16_t *)ppm_buffer,0,PPM_MAX_CHANNELS);
	ppm_last_count=0;
	/*�����ж�*/
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
}

/*******************************��������****************************************
* ��������: void PPM_Decode(uint32_t count)
* �������: count������������ֵ
* ���ز���: void
* ��    ��: PPM����
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void PPM_Decode(uint32_t count)
{
	uint16_t width;
	static uint8_t index = 0;
	/*���ϴ������ؿ�ʼ�����������ش����������������˶���*/
	width = count - ppm_last_count;

	if (count < ppm_last_count)
		width += PPM_MAX_COUNT;/*��Դ��������������ת�����*/

	ppm_last_count = count;

	if (width > PPM_MIN_START)
	{
		index = 0; /*�ҵ�ͬ��λ�ã���һ��������Ϊ��һͨ������ֵ*/
		ppm_frame_captured=SET;
	}
	else
	{
		if(index<=PPM_MAX_CHANNELS)/*��ֹ���ⴥ�������������*/
		{
			ppm_buffer[index++] = width;
		}
	}
}

/*******************************��������****************************************
* ��������: void PPM_RX_InterruptHandler(void)
* �������: void
* ���ز���: void
* ��    ��: PPM�жϴ�����
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void PPM_RX_InterruptHandler(void)
{
	ppm_count_temp = HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1);
	PPM_Decode(ppm_count_temp);
}

/*******************************��������****************************************
* ��������: void PPM_GetData(uint16_t *rc_pwm_in,uint8_t length)
* �������: rc_pwm_in����Ҫ��������ֵ,length:��Ҫ���������ݳ���
* ���ز���: void
* ��    ��: ��ȡPPM����������һ������
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
extern __IO ITStatus rc_newdata_flag;
void PPM_GetData(uint16_t *rc_pwm_in,uint8_t length)
{
	if(ppm_frame_captured == SET)
	{
		ppm_frame_captured=RESET;
		for (uint8_t i = 0; i < length; i++)
		{
			rc_pwm_in[i] = ppm_buffer[i];
		  rc_newdata_flag   = SET;
		}
	}
}

