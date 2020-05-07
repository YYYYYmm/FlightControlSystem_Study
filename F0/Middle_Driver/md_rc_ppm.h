/*******************************************************************************
* �ļ����ƣ�md_rc_ppm.h
*
* ժ    Ҫ��ң����PPM�������ͷ�ļ�
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/15
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#ifndef __MD_RC_PPM_H
#define __MD_RC_PPM_H

#include "stm32f1xx_hal.h"
#include "main.h"

#define PPM_MAX_COUNT     65535
#define PPM_MAX_CHANNELS  14
#define PPM_MIN_START		  3000
void PPM_Decode(uint32_t count);
void PPM_Init(void);
void PPM_RX_InterruptHandler(void);
void PPM_GetData(uint16_t *rc_pwm_in,uint8_t length);
#endif
