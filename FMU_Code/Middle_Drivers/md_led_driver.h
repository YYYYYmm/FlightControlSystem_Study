/*******************************************************************************
* �ļ����ƣ�md_led_driver.h
*
* ժ    Ҫ��STM32F1��LED�м������㣨�ײ�Ӳ�����������Ӧ�ò�֮��Ĳ㣩
*
* ��ǰ�汾��
* ��    �ߣ�
* ��    �ڣ�2017/12/18
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#ifndef __MD_LED_DRIVER_H
#define __MD_LED_DRIVER_H

#include "md_struct.h"

enum LED_STATE
{
	Red_OFF_Light=1, 
	Red_Fast_Flashing, 
	Red_Slow_Flashing, 
	Red_ON_Lighting, 
};

void MD_LED_AMBER_Control(uint8_t status);
void LED_Handler(void);
void set_mRed_State(enum LED_STATE state);

#endif
