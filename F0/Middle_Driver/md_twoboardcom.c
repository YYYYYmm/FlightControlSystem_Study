/*******************************************************************************
* �ļ����ƣ�md_ACROSScom.c
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
#include "md_twoboardcom.h"
#include "ringbuffer.h"
#include <string.h>
#include "checksum.h"
#include "md_struct.h"
/*
���ݰ���ʽ
uint8_t head1;      //����ͷ1 ��0XAA��
uint8_t head2;      //����ͷ2 ��0X55��
uint8_t length;     //������Ч���ȣ�payload + checksum��
uint8_t payload[n]  //��Ϣ������
uint16_t cjecksum;  //16λcrcУ��; �߰�λ��ǰ��
*/

/*
payload[n] ��Ϣ������
uint8_t[0];         //type of payload
uint8_t[1...n];     //details of payload
*/

/*****************************************************************************/
/*
  ���ݰ���ʽ����

	����ͷ1    ����ͷ2   ��Ч����     ����1---����n    CRCУ��Ͱ�λ    CRCУ��߰�λ
	0xAA       0x55      n+2         0xXX----0xXX     0xXX             0xXX
*/
/*****************************************************************************/

/*�������ݻ���������*/
uint8_t   m_Com_Page_Transmit[TRANSMIT_LENGTH];


/*�������������*/
//ringbuffer�������
RingBuffer  m_Com_RX_RingBuffMgr;
//�������ݴ�ű���
uint8_t     m_Com_RX_SaveBuff[BUFFER_LENGTH];
//������ݰ��������Ч���ݳ���
uint8_t     m_Com_Page_Length = 0;
uint8_t     m_Com_Page_Incres = 0;
uint8_t     m_Com_Page_Data[100];
enum Com_States e_ComStates = searchingHead1;
extern UART_HandleTypeDef huart2;
extern __IO ITStatus f_ComUartTxReady;

__IO ITStatus f_ComUartTxReady = SET;
__IO ITStatus f_ComUartRxReady = SET;
uint8_t twoboard_rx_data;

/*******************************��������****************************************
* ��������: void Com_TwoBoard_Transmit(void)
* �������:
* ���ز���:
* ��    ��: �����ݷ��͵���һ�˴�����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_Finalize_Message_Send(uint8_t* buf ,uint8_t length)
{
	HAL_UART_Transmit(&huart2,buf,length,200);
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_RX_Enable(void)
* �������:
* ���ز���:
* ��    ��: DMA����ʹ��
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RX_Enable(void)
{
	HAL_UART_Receive_DMA(&huart2, &twoboard_rx_data,1);
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_RX_InterruptHandler(void)
* �������:
* ���ز���:
* ��    ��: �����жϴ�����
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RX_InterruptHandler(void)
{
	f_ComUartRxReady = SET;
	Com_TwoBoard_RB_Push(twoboard_rx_data);
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_TX_InterruptHandler(void)
* �������:
* ���ز���:
* ��    ��: �����жϴ�������SET��־
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_TX_InterruptHandler(void)
{
	f_ComUartTxReady = SET;
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_RB_Init(void)
* �������:
* ���ز���:
* ��    ��: ��ʼ��һ��ѭ�����У�����������յ��Ĵ������ݡ���ʵ����һ�����ݻ�����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RB_Init(void)
{
	//��m_Com_RX_SaveBuff����m_Com_RX_RingBuffMgrѭ�����н��й�������
	rbInit(&m_Com_RX_RingBuffMgr, m_Com_RX_SaveBuff, BUFFER_LENGTH);
}

/*******************************��������****************************************
* ��������: uint8_t Com_TwoBoard_RB_IsReadyToRead(void)
* �������:
* ���ز���:  ����������򷵻�1����֮Ϊ0
* ��    ��: �ж�ringbuffer�����������Ƿ��н��յ�δ���������
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
uint8_t Com_TwoBoard_RB_IsReadyToRead(void)
{
	return !rbIsEmpty(&m_Com_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_RB_Clear(void)
* �������:
* ���ز���:
* ��    ��: ����ringbuffer��������á�
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RB_Clear(void)
{
	rbClear(&m_Com_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint8_t  Com_TwoBoard_RB_IsOverFlow(void)
* �������:
* ���ز���:  ���Ϊ1����֮Ϊ0
* ��    ��: �жϻ������Ƿ����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
uint8_t  Com_TwoBoard_RB_IsOverFlow(void)
{
	return m_Com_RX_RingBuffMgr.flagOverflow;
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_RB_Push(uint8_t data)
* �������: data����ѹ�������
* ���ز���:
* ��    ��: �����յ�����ѹ�뻺����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RB_Push(uint8_t data)
{
	rbPush(&m_Com_RX_RingBuffMgr, (uint8_t)(data & (uint8_t)0xFFU));
}

/*******************************��������****************************************
* ��������: uint8_t Com_TwoBoard_RB_Pop(void)
* �������:
* ���ز���: uint8_t ����������
* ��    ��: �ӻ�������������
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
uint8_t Com_TwoBoard_RB_Pop(void)
{
	return rbPop(&m_Com_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint8_t Com_TwoBoard_RB_HasNew(void)
* �������:
* ���ز���:
* ��    ��: �ж��Ƿ����µ�����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
uint8_t Com_TwoBoard_RB_HasNew(void)
{
	return !rbIsEmpty(&m_Com_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: void Clear_Page_DatasArea(void)
* �������:
* ���ز���:
* ��    ��: ������ر�־���������ݵ�����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Clear_Page_DatasArea(void)
{
	m_Com_Page_Length=0;
	m_Com_Page_Incres = 0;
	//��������
	memset(m_Com_Page_Data,0, sizeof(m_Com_Page_Data));
}

/*******************************��������****************************************
* ��������: uint8_t CheckSUM(uint8_t * data ,uint8_t length)
* �������: data����У������ݰ� �� length�����ݰ���Чλ����
* ���ز���: �������һ�·���1����һ�·���0 ��
* ��    ��: �������ݰ������У�������λ�Ƿ�һ�¡�
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
uint8_t CheckSUM(uint8_t * data ,uint8_t length)
{
	uint16_t CheckSum_Cacl = 0;
	uint16_t CheckSum_Recive;
	CheckSum_Recive += (uint16_t)(data[length-1]);
	CheckSum_Recive += (uint16_t)(data[length-2] <<8);
	for(uint8_t count=0; count<length; count++)
	{
		CheckSum_Cacl +=data[count];
	}
	if(CheckSum_Cacl == CheckSum_Recive)
	{
		return 1;
	}
	else
	{
		return 0 ;
	}
}


/*******************************��������****************************************
* ��������: void extra_pwm_value(uint8_t *buf)
* �������:
* ���ز���:
* ��    ��: ��PWMֵ������ȡ����
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
Motor_Value m_Motor_From_F4;
void extra_pwm_value(uint8_t *buf)
{
	UINT16_8BIT m_Value;
	m_Value.value_8[0]=buf[0];
	m_Value.value_8[1]=buf[1];
	m_Motor_From_F4.motor1 = m_Value.value_16;

	m_Value.value_8[0]=buf[2];
	m_Value.value_8[1]=buf[3];
	m_Motor_From_F4.motor2 = m_Value.value_16;

	m_Value.value_8[0]=buf[4];
	m_Value.value_8[1]=buf[5];
	m_Motor_From_F4.motor3 = m_Value.value_16;

	m_Value.value_8[0]=buf[6];
	m_Value.value_8[1]=buf[7];
	m_Motor_From_F4.motor4 = m_Value.value_16;

	m_Value.value_8[0]=buf[8];
	m_Value.value_8[1]=buf[9];
	m_Motor_From_F4.motor5 = m_Value.value_16;

	m_Value.value_8[0]=buf[10];
	m_Value.value_8[1]=buf[11];
	m_Motor_From_F4.motor6 = m_Value.value_16;
}

uint8_t flag_heartbeat=0; //��������������һ��������Ź�����ʱ���״̬
void decode_message(uint8_t *buf_msg,uint8_t length)
{
	//������Ϣ���������ж���ʲô����
	switch (buf_msg[0])
	{
	case HEARTBEAT_TYPE:
		flag_heartbeat=1;
		break;
	case RCINPUT_TYPE:
		break;
	case PWM_TYPE:
		extra_pwm_value(&buf_msg[1]);
		break;

	default:
		break;
	}
}
/*******************************��������****************************************
* ��������: void Com_TwoBoard_RB_Decode(void)
* �������:
* ���ز���:
* ��    ��: ����ȡ��������
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RB_Decode(void)
{
	uint16_t checksum_rx_temp =0;
	/*������ݰ�buffer��������ˣ������㣬���¼���*/
	if(Com_TwoBoard_RB_IsOverFlow())
	{
		Com_TwoBoard_RB_Clear();
	}

	while(Com_TwoBoard_RB_HasNew())
	{
		//��ȡ����
		uint8_t cur = Com_TwoBoard_RB_Pop();
		//״̬����ʽ��������
		switch (e_ComStates)
		{
		case searchingHead1:
			//Ѱ������ͷ1
			if(cur == COM_HEAD1)
			{
				//�ҵ�����ͷ1 ����һ��������ת��searchingHead2
				e_ComStates = searchingHead2;
			}
			break;
		case searchingHead2:
			if(cur == COM_HEAD2)
			{
				//�ҵ�����ͷ2�������receiveDateArea
				e_ComStates = record_VaildLenght;
			}
			else
			{
				//������ݶ�Ӧ���ϣ�����ת��searchingHead1����Ѱ��
				e_ComStates = searchingHead1;
			}
			break;

		case record_VaildLenght:
			//��¼���ݰ���Ч���ݳ���
			m_Com_Page_Length = cur;
			//�������ݼ�¼��־��׼����¼���ݡ�
			m_Com_Page_Incres = 0;
			m_Com_Page_Data[m_Com_Page_Incres++] = cur;
			e_ComStates = record_VaildDatas;
			break;

		case record_VaildDatas:
			//���μ�¼����
			m_Com_Page_Data[m_Com_Page_Incres++] = cur;
			//�ж������Ƿ�ȫ���������
			if(m_Com_Page_Incres==m_Com_Page_Length+1)
			{
				e_ComStates = decode_Page;
			}
			break;

		case decode_Page:
			m_Com_Page_Data[m_Com_Page_Incres++] = cur;
			if(m_Com_Page_Incres==(m_Com_Page_Length+3))
			{
				//1.����֤�����Ƿ���ȷ
				checksum_rx_temp = (uint16_t)(m_Com_Page_Data[m_Com_Page_Length+2]<<8) | (m_Com_Page_Data[m_Com_Page_Length+1]);
				if(crc_calculate (&m_Com_Page_Data[0],m_Com_Page_Length+BYTES_OF_LENGTH)==checksum_rx_temp)
				{
					//2.�����ȷ����룻����ȷֱ�Ӷ���
					decode_message(&m_Com_Page_Data[1],m_Com_Page_Length);
				}
				//3.�����־�����ݽ�������׼����һ�����ݽ���
				Clear_Page_DatasArea();
				e_ComStates=searchingHead1;
			}
			break;

		default:
			e_ComStates=searchingHead1;
			Clear_Page_DatasArea();
			break;
		}
	}
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_RB_Update(void)
* �������:
* ���ز���:
* ��    ��: ������յ����ݵĺ������˺�����main�����е��á�
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_RB_Update(void)
{
	if(Com_TwoBoard_RB_IsReadyToRead())
	{
		Com_TwoBoard_RB_Decode();
	}
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_Msg_HeartBeat_Send(void)
* �������:
* ���ز���:
* ��    ��: �������ķ��ͣ�ȷ��˫��ͨ��ͨ��û���ϰ����ѹ̶�Ƶ�ʷ��͡�����ȫ���ص��ź�״̬���������������
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_Msg_HeartBeat_Send(uint8_t safeSwitch)
{
	/*�����ݷ��������������*/
	uint16_t checksum;
	uint8_t buf[HEARTBEAT_BUF_LENGTH]= {0};
	//����ͷ
	_twoboard_put_uint8_t(buf,0,COM_HEAD1);
	_twoboard_put_uint8_t(buf,1,COM_HEAD2);

	//��Ϣ����Ч����
	_twoboard_put_uint8_t(buf,2,HEARTBEAT_VALID_DATA_LENGTH);

	//��Ϣ������
	_twoboard_put_uint8_t(buf,3,HEARTBEAT_TYPE);
	//��Ϣ������
	_twoboard_put_uint8_t(buf,4,safeSwitch);

	//����CRCУ��
	checksum = crc_calculate(&buf[2],HEARTBEAT_VALID_DATA_LENGTH+BYTES_OF_LENGTH);
	_twoboard_put_uint16_t(buf,5,checksum);

	Com_TwoBoard_Finalize_Message_Send(buf,HEARTBEAT_BUF_LENGTH);
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_Msg_RCinput_Send(uint8_t *rc_input)
* �������:
* ���ز���:
* ��    ��: ����ң�����Ľ���������
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_Msg_RCinput_Send(uint16_t *rc_input)
{
	/*�����ݷ��������������*/
	uint16_t checksum;
	uint8_t buf[RCINPUT_BUF_LENGTH]= {0};
	//����ͷ
	_twoboard_put_uint8_t(buf,0,COM_HEAD1);
	_twoboard_put_uint8_t(buf,1,COM_HEAD2);

	//��Ϣ����Ч����
	_twoboard_put_uint8_t(buf,2,RCINPUT_VALID_DATA_LENGTH);

	//��Ϣ������
	_twoboard_put_uint8_t(buf,3,RCINPUT_TYPE);
	//��Ϣ������
	_twoboard_put_uint16_t(buf,4,rc_input[0]);
	_twoboard_put_uint16_t(buf,6,rc_input[1]);
	_twoboard_put_uint16_t(buf,8,rc_input[2]);
	_twoboard_put_uint16_t(buf,10,rc_input[3]);
	_twoboard_put_uint16_t(buf,12,rc_input[4]);
	_twoboard_put_uint16_t(buf,14,rc_input[5]);
	_twoboard_put_uint16_t(buf,16,rc_input[6]);
	_twoboard_put_uint16_t(buf,18,rc_input[7]);

	//����CRCУ��
	checksum = crc_calculate(&buf[2],RCINPUT_VALID_DATA_LENGTH+BYTES_OF_LENGTH);
	_twoboard_put_uint16_t(buf,20,checksum);

	Com_TwoBoard_Finalize_Message_Send(buf,RCINPUT_BUF_LENGTH);
}

/*******************************��������****************************************
* ��������: void Com_TwoBoard_Init(void)
* �������:
* ���ز���:
* ��    ��: ��ʼ��˫��ͨ�ŵ��������
* ��    ��: by ACROSS
* ��    ��: 2017/12/18
*******************************************************************************/
void Com_TwoBoard_Init(void)
{
	Com_TwoBoard_RB_Init();
	Com_TwoBoard_RX_Enable();
}
