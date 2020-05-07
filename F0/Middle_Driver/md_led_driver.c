/*******************************************************************************
* �ļ����ƣ�md_led_driver.c
*
* ժ    Ҫ��STM32F1��LED�м�������ʵ�֣��ײ�Ӳ�����������Ӧ�ò�֮��Ĳ㣩
*
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
* ��������: void MD_LED_BLUE_Control(uint8_t status)
* �������: uint8_t status�����ƵƵ�����״̬�� 1��������0��Ϩ��
* ���ز���: void
* ��    ��: ����blue�Ƶ�����
* ��    ��: xxx
* ��    ��: 2018.3.18
*******************************************************************************/
void MD_LED_Bule_Control(uint8_t status)
{
	if(status==1)
	{
		//����
		HAL_GPIO_WritePin(IO_LED_BLUE_GPIO_Port,IO_LED_BLUE_Pin,GPIO_PIN_RESET);
	}
	else
	{
		//Ϩ��
		HAL_GPIO_WritePin(IO_LED_BLUE_GPIO_Port,IO_LED_BLUE_Pin,GPIO_PIN_SET);
	}
}

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
		HAL_GPIO_WritePin(IO_LED_AMBER_GPIO_Port,IO_LED_AMBER_Pin,GPIO_PIN_RESET);
	}
	else
	{
		//Ϩ��
		HAL_GPIO_WritePin(IO_LED_AMBER_GPIO_Port,IO_LED_AMBER_Pin,GPIO_PIN_SET);
	}
}

/*******************************��������****************************************
* ��������: void MD_LED_AMBER_Control(uint8_t status)
* �������: status�����ƵƵ�����״̬
* ���ز���:
* ��    ��:
* ��    ��:
* ��    ��: 2017/12/18
*******************************************************************************/
void MD_LED_SAFETY_Control(uint8_t status)
{
	if(status==1)
	{
		//����
		HAL_GPIO_WritePin(IO_LED_SAFETY_GPIO_Port,IO_LED_SAFETY_Pin,GPIO_PIN_RESET);
	}
	else
	{
		//Ϩ��
		HAL_GPIO_WritePin(IO_LED_SAFETY_GPIO_Port,IO_LED_SAFETY_Pin,GPIO_PIN_SET);
	}
}
