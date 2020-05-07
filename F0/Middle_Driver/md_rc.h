/*******************************************************************************
* �ļ����ƣ�md_rc.h
*
* ժ    Ҫ��ң�������ͷ�ļ�
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/15
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#ifndef __MD_RC_H
#define __MD_RC_H

#include "stm32f1xx_hal.h"
#include "main.h"

#define RC_INPUT_CHANNELS  8

extern __IO uint8_t rc_frame_byte;
extern uint16_t rc_pwm_in[RC_INPUT_CHANNELS];

void RC_Input_Init(void);
void RC_Input_Loop(void);

#endif

