
/*******************************************************************************
* �ļ����ƣ�md_config.h
*
* ժ    Ҫ���м��������һЩ����
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/15
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#ifndef __MD_CONFIG_H
#define __MD_CONFIG_H

/*
ң����������Ϣ,����ѡ��һ
*/
//#define RC_INPUT_PPM  /*ʹ��PPM�ź���Ϊң��������*/
#define RC_INPUT_SBUS /*ʹ��SBUS�ź���Ϊң��������*/

#if defined (RC_INPUT_PPM) && defined (RC_INPUT_SBUS)
#error "These two macros (RC_INPUT_PPM,RC_INPUT_SBUS) cannot be defined at the same time!"
#endif

#if !defined (RC_INPUT_PPM) && !defined (RC_INPUT_SBUS)
#error "Please define a macro to decide which type of RC that be used!"
#endif

#endif






