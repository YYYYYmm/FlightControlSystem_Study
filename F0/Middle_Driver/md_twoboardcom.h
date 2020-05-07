/*******************************************************************************
* �ļ����ƣ�md_twoboardcom.h
*
* ժ    Ҫ��˫��ͨ����صĴ������ü�ѭ�����е�ʹ��
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2017/12/18
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#ifndef __MD_TWOBOARDCOM_H
#define __MD_TWOBOARDCOM_H

#include "stm32f1xx_hal.h"

enum Com_States {searchingHead1,searchingHead2,record_VaildLenght,record_VaildDatas,decode_Page};

//ringbuffer�������ĳ���
#define BUFFER_LENGTH    100
//�������黺�����ĳ���
#define TRANSMIT_LENGTH  100
#define COM_HEAD1        0XAA
#define COM_HEAD2        0X55
#define BYTES_OF_LENGTH  1

#define HEARTBEAT_BUF_LENGTH   7
#define HEARTBEAT_VALID_DATA_LENGTH   2
#define HEARTBEAT_TYPE  0

#define RCINPUT_BUF_LENGTH   22
#define RCINPUT_VALID_DATA_LENGTH   17
#define RCINPUT_TYPE  1

#define PWM_TYPE  2

#define _twoboard_put_uint8_t(buf, wire_offset, b)  buf[wire_offset] = (uint8_t)b
#define _twoboard_int8_t(buf, wire_offset, b)       buf[wire_offset] = (int8_t)b
#define _twoboard_put_uint16_t(buf, wire_offset, b) *(uint16_t *)&buf[wire_offset] = b
#define _twoboard_put_int16_t(buf, wire_offset, b)  *(int16_t *)&buf[wire_offset] = b
#define _twoboard_put_uint32_t(buf, wire_offset, b) *(uint32_t *)&buf[wire_offset] = b
#define _twoboard_put_int32_t(buf, wire_offset, b)  *(int32_t *)&buf[wire_offset] = b
#define _twoboard_put_uint64_t(buf, wire_offset, b) *(uint64_t *)&buf[wire_offset] = b
#define _twoboard_put_int64_t(buf, wire_offset, b)  *(int64_t *)&buf[wire_offset] = b
#define _twoboard_put_float(buf, wire_offset, b)    *(float *)&buf[wire_offset] = b
#define _twoboard_put_double(buf, wire_offset, b)   *(double *)&buf[wire_offset] = b

void Com_TwoBoard_Finalize_Message_Send(uint8_t* buf ,uint8_t length);
void Com_TwoBoard_RX_InterruptHandler(void);
void Com_TwoBoard_TX_InterruptHandler(void);

void Com_TwoBoard_RB_Init(void);
void Com_TwoBoard_RB_Update(void);
void Com_TwoBoard_RB_Push(uint8_t data);

void Com_TwoBoard_Msg_HeartBeat_Send(uint8_t safeSwitch);
void Com_TwoBoard_Msg_RCinput_Send(uint16_t *rc_input);

void Com_TwoBoard_Init(void);
#endif
