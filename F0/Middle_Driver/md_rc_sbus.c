/*******************************************************************************
* �ļ����ƣ�md_rc_sbus.h
*
* ժ    Ҫ��ң����sbus�������C�ļ�
*
* ��ǰ�汾��
* ��    �ߣ�ACROSS
* ��    �ڣ�2018/05/15
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/
#include "md_rc_sbus.h"
#include "usart.h"
#include "dma.h"

/* sbus values */
__IO uint32_t cur_time=0,last_time=0;
 uint8_t rc_frame_byte;

uint8_t sbus_frame[25];
uint16_t sbus_value[18];
__IO uint8_t sbus_bytecnt = 0;
uint8_t sbus_failsafe = 0, sbus_framedrop = 0;

__IO ITStatus rc_frame_captured = RESET;

/*
 * S.bus decoder matrix.
 *
 * Each channel value can come from up to 3 input bytes. Each row in the
 * matrix describes up to three bytes, and each entry gives:
 *
 * - byte offset in the data portion of the frame
 * - right shift applied to the data byte
 * - mask for the data byte
 * - left shift applied to the result into the channel value
 */
struct sbus_bit_pick
{
	uint8_t byte;
	uint8_t rshift;
	uint8_t mask;
	uint8_t lshift;
};
static const struct sbus_bit_pick SBUS_Decoder[SBUS_INPUT_CHANNELS][3] =
{
	/*  0 */ { { 0, 0, 0xff, 0}, { 1, 0, 0x07, 8}, { 0, 0, 0x00,  0} },
	/*  1 */ { { 1, 3, 0x1f, 0}, { 2, 0, 0x3f, 5}, { 0, 0, 0x00,  0} },
	/*  2 */ { { 2, 6, 0x03, 0}, { 3, 0, 0xff, 2}, { 4, 0, 0x01, 10} },
	/*  3 */ { { 4, 1, 0x7f, 0}, { 5, 0, 0x0f, 7}, { 0, 0, 0x00,  0} },
	/*  4 */ { { 5, 4, 0x0f, 0}, { 6, 0, 0x7f, 4}, { 0, 0, 0x00,  0} },
	/*  5 */ { { 6, 7, 0x01, 0}, { 7, 0, 0xff, 1}, { 8, 0, 0x03,  9} },
	/*  6 */ { { 8, 2, 0x3f, 0}, { 9, 0, 0x1f, 6}, { 0, 0, 0x00,  0} },
	/*  7 */ { { 9, 5, 0x07, 0}, {10, 0, 0xff, 3}, { 0, 0, 0x00,  0} },
	/*  8 */ { {11, 0, 0xff, 0}, {12, 0, 0x07, 8}, { 0, 0, 0x00,  0} },
	/*  9 */ { {12, 3, 0x1f, 0}, {13, 0, 0x3f, 5}, { 0, 0, 0x00,  0} },
	/* 10 */ { {13, 6, 0x03, 0}, {14, 0, 0xff, 2}, {15, 0, 0x01, 10} },
	/* 11 */ { {15, 1, 0x7f, 0}, {16, 0, 0x0f, 7}, { 0, 0, 0x00,  0} },
	/* 12 */ { {16, 4, 0x0f, 0}, {17, 0, 0x7f, 4}, { 0, 0, 0x00,  0} },
	/* 13 */ { {17, 7, 0x01, 0}, {18, 0, 0xff, 1}, {19, 0, 0x03,  9} },
	/* 14 */ { {19, 2, 0x3f, 0}, {20, 0, 0x1f, 6}, { 0, 0, 0x00,  0} },
	/* 15 */ { {20, 5, 0x07, 0}, {21, 0, 0xff, 3}, { 0, 0, 0x00,  0} }
};

/*******************************��������****************************************
* ��������: void SBUS_Init(void)
* �������: void
* ���ز���: void
* ��    ��: SBUS��صı�����ʼ������DMAʹ��
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void SBUS_Init(void)
{
	/*����SBUS��DMA�жϽ���*/
	HAL_UART_Receive_DMA(&huart3,&rc_frame_byte, 1);
	cur_time = HAL_GetTick();
	last_time = cur_time;
}

/*******************************��������****************************************
* ��������: void SBUS_Decode(uint8_t *frame, uint16_t *values, uint8_t *sbus_failsafe, uint8_t *sbus_frame_drop,
                    uint16_t max_values)
* �������: void
* ���ز���: void
* ��    ��: SBUS��صı�����ʼ������DMAʹ��
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void SBUS_Decode(uint8_t *frame, uint16_t *values, uint8_t *sbus_failsafe, uint8_t *sbus_frame_drop,
                 uint16_t max_values)
{
	switch (frame[24])
	{
	case 0x00:
		/* this is S.BUS 1 */
		break;

	case 0x03:
		/* S.BUS 2 SLOT0: RX battery and external voltage */
		break;

	case 0x83:
		/* S.BUS 2 SLOT1 */
		break;

	case 0x43:
	case 0xC3:
	case 0x23:
	case 0xA3:
	case 0x63:
	case 0xE3:
		break;

	default:
		/* we expect one of the bits above, but there are some we don't know yet */
		break;
	}

	unsigned chancount = (max_values > SBUS_INPUT_CHANNELS) ?
	                     SBUS_INPUT_CHANNELS : max_values;

	/* use the decoder matrix to extract channel data */
	for (unsigned channel = 0; channel < chancount; channel++)
	{
		unsigned value = 0;

		for (unsigned pick = 0; pick < 3; pick++)
		{
			const struct sbus_bit_pick *decode = &SBUS_Decoder[channel][pick];

			if (decode->mask != 0)
			{
				unsigned piece = frame[1 + decode->byte];
				piece >>= decode->rshift;
				piece &= decode->mask;
				piece <<= decode->lshift;

				value |= piece;
			}
		}


		/* convert 0-2048 values to 1000-2000 ppm encoding in a not too sloppy fashion */
		values[channel] = (uint16_t)(value * SBUS_SCALE_FACTOR + .5f) + SBUS_SCALE_OFFSET;
	}

	/* decode switch channels if data fields are wide enough */
	if (chancount > 15)
	{
		chancount = 18;

		/* channel 17 (index 16) */
		values[16] = (frame[SBUS_FLAGS_BYTE] & (1 << 0)) * 1000 + 998;
		/* channel 18 (index 17) */
		values[17] = (frame[SBUS_FLAGS_BYTE] & (1 << 1)) * 1000 + 998;
	}

	/* decode and handle failsafe and frame-lost flags */
	if (frame[SBUS_FLAGS_BYTE] & (1 << SBUS_FAILSAFE_BIT))   /* failsafe */
	{
		/* report that we failed to read anything valid off the receiver */
		*sbus_failsafe = 1;
		*sbus_frame_drop = 1;

	}
	else if (frame[SBUS_FLAGS_BYTE] & (1 << SBUS_FRAMELOST_BIT))     /* a frame was lost */
	{
		/* set a special warning flag
		 *
		 * Attention! This flag indicates a skipped frame only, not a total link loss! Handling this
		 * condition as fail-safe greatly reduces the reliability and range of the radio link,
		 * e.g. by prematurely issueing return-to-launch!!! */

		*sbus_failsafe = 0;
		*sbus_frame_drop = 1;

	}
	else
	{
		*sbus_failsafe = 0;
		*sbus_frame_drop = 0;
	}

}

/*******************************��������****************************************
* ��������: void SBUS_RX_InterruptHandler(void)
* �������: void
* ���ز���: void
* ��    ��: SBUS�����жϴ�����
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
void SBUS_RX_InterruptHandler(void)
{
	cur_time = HAL_GetTick();
	/*�������ݰ�֮��ļ��ʱ��������֡ͷ*/
	if (cur_time - last_time > SBUS_PACKAGE_INTERVAL_TIME)
	{
		if (sbus_bytecnt == 25 && sbus_frame[0] == 0x0f && sbus_frame[24] == 0x00)
		{
			rc_frame_captured = SET; /*����һ�����ݰ��������*/
		}
		sbus_bytecnt = 0;
	}
  last_time = cur_time;
	sbus_frame[sbus_bytecnt++] = rc_frame_byte;
}

/*******************************��������****************************************
* ��������: void SBUS_GetData(uint16_t *rc_pwm_in,uint8_t lenght)
* �������: rc_pwm_in����Ҫ����������;length:��Ҫ��ȡ�ĳ���
* ���ز���: void
* ��    ��: SBUS��ȡ���µ�����
* ��    ��: ACROSS
* ��    ��: 2018.05.15
*******************************************************************************/
extern __IO ITStatus rc_newdata_flag;
void SBUS_GetData(uint16_t *rc_pwm_in,uint8_t lenght)
{
	if(rc_frame_captured==SET)
	{
		rc_frame_captured=RESET;
		SBUS_Decode(sbus_frame, sbus_value, &sbus_failsafe, &sbus_framedrop, (uint16_t)SBUS_MAX_VALUES);
		for (uint8_t i = 0; i < lenght; i++)
		{
			rc_pwm_in[i] = sbus_value[i];
			rc_newdata_flag=SET;
		}
	}
}
