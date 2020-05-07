/*******************************************************************************
* �ļ����ƣ�mavlink_main.c
*
* ժ    Ҫ��mavlink�Զ����ļ�
*
* ��ǰ�汾��
* ��    �ߣ�
* ��    �ڣ�2018/05/30
* ���뻷����keil5
*
* ��ʷ��Ϣ��
*******************************************************************************/

#include "mavlink.h"
#include "mavlink_types.h"

#include "usart.h"
#include "md_struct.h"
#include "ringbuffer.h"
#include "stdio.h"
#include "sensor.h"
#include "attitude_estimator_mahony.h"

#define  MAVLINK_READ_BUFFER_SIZE 128

/*������������*/
//ringbuffer�������
RingBuffer  m_Mavlink_RX_RingBuffMgr;
/*MAVLINK�������ݻ���������*/
uint8_t   m_Mavlink_RX_Buff[MAVLINK_READ_BUFFER_SIZE*10];
uint8_t mavlink_byte;
/*******************************��������****************************************
* ��������: void Mavlink_RB_Init(void)
* �������:
* ���ز���:
* ��    ��: ��ʼ��һ��ѭ�����У�����������յ��Ĵ������ݡ���ʵ����һ�����ݻ�����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Init(void)
{
	//��m_Mavlink_RX_Buffm_Mavlink_RX_RingBuffMgr�����н��й�������
	rbInit(&m_Mavlink_RX_RingBuffMgr, m_Mavlink_RX_Buff, sizeof(m_Mavlink_RX_Buff));
}

/*******************************��������****************************************
* ��������: void Mavlink_RB_Clear(void)
* �������:
* ���ز���:
* ��    ��: ����ringbuffer��������á�
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Clear(void)
{
	rbClear(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint8_t  Mavlink_RB_IsOverFlow(void)
* �������:
* ���ز���:  ���Ϊ1����֮Ϊ0
* ��    ��: �жϻ������Ƿ����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint8_t  Mavlink_RB_IsOverFlow(void)
{
	return m_Mavlink_RX_RingBuffMgr.flagOverflow;
}

/*******************************��������****************************************
* ��������: void Mavlink_RB_Push(uint8_t data)
* �������: data����ѹ�������
* ���ز���:
* ��    ��: �����յ�����ѹ�뻺����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_RB_Push(uint8_t data)
{
	rbPush(&m_Mavlink_RX_RingBuffMgr, (uint8_t)(data & (uint8_t)0xFFU));
}

/*******************************��������****************************************
* ��������: uint8_t Mavlink_RB_Pop(void)
* �������:
* ���ز���: uint8_t ����������
* ��    ��: �ӻ�������������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint8_t Mavlink_RB_Pop(void)
{
	return rbPop(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint8_t Mavlink_RB_HasNew(void)
* �������:
* ���ز���:
* ��    ��: �ж��Ƿ����µ�����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint8_t Mavlink_RB_HasNew(void)
{
	return !rbIsEmpty(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: uint16_t Mavlink_RB_Count(void)
* �������:
* ���ز���:
* ��    ��: �ж��ж��ٸ�������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
uint16_t Mavlink_RB_Count(void)
{
	return rbGetCount(&m_Mavlink_RX_RingBuffMgr);
}

/*******************************��������****************************************
* ��������: void Mavlink_Rece_Enable(void)
* �������:
* ���ز���:
* ��    ��: ʹ��DMA����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_Rece_Enable(void)
{
	HAL_UART_Receive_DMA(&huart7, &mavlink_byte, 1);
}

/*******************************��������****************************************
* ��������: void Mavlink_Init(void)
* �������:
* ���ز���:
* ��    ��: ��ʼ��MAVLINK��ʹ�ܽ��գ�ringbuffer����
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_Init(void)
{
	Mavlink_Rece_Enable();
	Mavlink_RB_Init();
}

/*******************************��������****************************************
* ��������: void Mavlin_RX_InterruptHandler(void)
* �������:
* ���ز���:
* ��    ��: �����жϵĴ���������Ҫ�ǽ�����ѹ��ringbuffer������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlin_RX_InterruptHandler(void)
{
	//����ѹ��
	rbPush(&m_Mavlink_RX_RingBuffMgr,mavlink_byte);
}
/*�ڡ�mavlink_helpers.h����Ҫʹ�á�*/
mavlink_system_t mavlink_system =
{
	1,
	1
}; // System ID, 1-255, Component/Subsystem ID, 1-255

/*******************************��������****************************************
* ��������: void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
* �������:
* ���ز���:
* ��    ��: ���¶���mavlink�ķ��ͺ�������ײ�Ӳ���ӿڹ�������
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length)
{
	HAL_UART_Transmit(&huart7, (uint8_t *)ch, length, 2000);
}

/*******************************��������****************************************
* ��������: void Mavlink_Msg_Handle(void)
* �������:
* ���ز���:
* ��    ��: �����QGC��λ����������������Ϣ
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
void Mavlink_Msg_Handle(mavlink_message_t msg)
{
	switch(msg.msgid)
	{
	case MAVLINK_MSG_ID_HEARTBEAT:
//		printf("this is heartbeat from QGC/r/n");
		break;
	case MAVLINK_MSG_ID_SYS_STATUS:
//		  osd_vbat = (mavlink_msg_sys_status_get_voltage_battery(&msg) / 1000.0f); //Battery voltage, in millivolts (1 = 1 millivolt)
//			osd_curr = mavlink_msg_sys_status_get_current_battery(&msg); //Battery current, in 10*milliamperes (1 = 10 milliampere)
//			osd_battery_remaining = mavlink_msg_sys_status_get_battery_remaining(&msg); //Remaining battery energy: (0%: 0, 100%: 100)
		break;
	default:
		break;
	}
}
/*******************************��������****************************************
* ��������: Loop_Mavlink_Parse(void)
* �������:
* ���ز���:
* ��    ��: ��main�����в���ϵ��ô˺���
* ��    ��: by Across
* ��    ��: 2018/06/02
*******************************************************************************/
mavlink_message_t msg;
mavlink_status_t status;

void Loop_Mavlink_Parse(void)
{
	if(Mavlink_RB_IsOverFlow())
	{
		Mavlink_RB_Clear();
	}

	while(Mavlink_RB_HasNew())
	{
		uint8_t read = Mavlink_RB_Pop();
		if(mavlink_parse_char(MAVLINK_COMM_0, read, &msg, &status))
		{
			//�źŴ�����
			Mavlink_Msg_Handle(msg);
			//printf("Received message with ID %d, sequence: %d from component %d of system %d", msg.msgid, msg.seq, msg.compid, msg.sysid);
		}
	}
}


/*���Ժ���������******************************************************************************/
extern MPU6000_Data 		m_Mpu6000;
extern MS56XX_Data 	  m_Ms56xx;
extern hmc5883MagData 	m_Hmc5883;

static void mavlink_test_heartbeat2(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_HEARTBEAT >= 256)
	{
		return;
	}
#endif
//	mavlink_message_t msg;
//	uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
//	uint16_t i;
	mavlink_heartbeat_t packet_in =
	{
		963497464,17,84,151,218,3
	};
	mavlink_heartbeat_t packet1, packet2;
	memset(&packet1, 0, sizeof(packet1));
	packet1.custom_mode = packet_in.custom_mode;
	packet1.type = packet_in.type;
	packet1.autopilot = packet_in.autopilot;
	packet1.base_mode = packet_in.base_mode;
	packet1.system_status = packet_in.system_status;
	packet1.mavlink_version = packet_in.mavlink_version;


#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_HEARTBEAT_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_HEARTBEAT_MIN_LEN);
	}
#endif
	memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_heartbeat_send(MAVLINK_COMM_0 , packet1.type , packet1.autopilot , packet1.base_mode , packet1.custom_mode , packet1.system_status );
	MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}


static void mavlink_test_raw_imu2(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_RAW_IMU >= 256)
	{
		return;
	}
#endif
//	mavlink_message_t msg;
//	uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
//	uint16_t i;
	mavlink_raw_imu_t packet1;
	memset(&packet1, 0, sizeof(packet1));
	packet1.time_usec =0123456;
	packet1.xacc = m_Mpu6000.acc_raw.x;
	packet1.yacc = m_Mpu6000.acc_raw.y;
	packet1.zacc = m_Mpu6000.acc_raw.z;
	packet1.xgyro = m_Mpu6000.gyro_raw.x;
	packet1.ygyro = m_Mpu6000.gyro_raw.y;
	packet1.zgyro = m_Mpu6000.gyro_raw.z;
	packet1.xmag = (int16_t)m_Hmc5883.x;
	packet1.ymag = (int16_t)m_Hmc5883.y;
	packet1.zmag = (int16_t)m_Hmc5883.z;

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_RAW_IMU_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_RAW_IMU_MIN_LEN);
	}
#endif

	mavlink_msg_raw_imu_send(MAVLINK_COMM_0 , packet1.time_usec , packet1.xacc , packet1.yacc , packet1.zacc , packet1.xgyro , packet1.ygyro , packet1.zgyro , packet1.xmag , packet1.ymag , packet1.zmag );
}

static void mavlink_test_highres_imu(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_RAW_IMU >= 256)
	{
		return;
	}
#endif
//	mavlink_message_t msg;
//	uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
//	uint16_t i;
	mavlink_highres_imu_t packet1;
	memset(&packet1, 0, sizeof(packet1));
	packet1.time_usec =0123456;
	packet1.xacc = Ctrl_state.accf.x;
	packet1.yacc = Ctrl_state.accf.y;
	packet1.zacc = Ctrl_state.accf.z;
	packet1.xgyro = Ctrl_state.gyrof.x;
	packet1.ygyro = Ctrl_state.gyrof.y;
	packet1.zgyro = Ctrl_state.gyrof.z;
	packet1.xmag = Ctrl_state.magf.x;
	packet1.ymag = Ctrl_state.magf.y;
	packet1.zmag = Ctrl_state.magf.z;
	packet1.abs_pressure = 0.0f;
	packet1.diff_pressure = 0.0f;
	packet1.pressure_alt = 0.0f;
	packet1.temperature = 0.0f;
	packet1.fields_updated = 1;

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_RAW_IMU_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_RAW_IMU_MIN_LEN);
	}
#endif

	mavlink_msg_highres_imu_send(MAVLINK_COMM_1 , packet1.time_usec , packet1.xacc , packet1.yacc , packet1.zacc , packet1.xgyro , packet1.ygyro , packet1.zgyro , packet1.xmag , packet1.ymag , packet1.zmag ,
				packet1.abs_pressure , packet1.diff_pressure , packet1.pressure_alt , packet1.temperature , packet1.fields_updated );
}


static void mavlink_test_attitude(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_RAW_IMU >= 256)
	{
		return;
	}
#endif
//	mavlink_message_t msg;
//        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
//        uint16_t i;
    mavlink_attitude_t packet_in = {
        963497464,45.0,73.0,101.0,129.0,157.0,185.0
    };
    mavlink_attitude_t packet1;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_boot_ms = packet_in.time_boot_ms;
        packet1.roll = attitude_roll;
        packet1.pitch = attitude_pitch;
        packet1.yaw = attitude_yaw;
        packet1.rollspeed = Ctrl_state.gyrof.x;
        packet1.pitchspeed = Ctrl_state.gyrof.y;
        packet1.yawspeed = Ctrl_state.gyrof.z;

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_RAW_IMU_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_RAW_IMU_MIN_LEN);
	}
#endif

	mavlink_msg_attitude_send(MAVLINK_COMM_1 , packet1.time_boot_ms , packet1.roll , packet1.pitch , packet1.yaw , packet1.rollspeed , packet1.pitchspeed , packet1.yawspeed );
}

static void mavlink_test_local_position_ned(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
	if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_RAW_IMU >= 256)
	{
		return;
	}
#endif
//	mavlink_message_t msg;
//        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
//        uint16_t i;
//    mavlink_local_position_ned_t packet_in = {
//        963497464,45.0,73.0,101.0,129.0,157.0,185.0
//    };
    mavlink_local_position_ned_t packet1;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_boot_ms = 222;
        packet1.x = Ctrl_state.x;
        packet1.y = Ctrl_state.y;
        packet1.z = Ctrl_state.z;
        packet1.vx = Ctrl_state.vx;
        packet1.vy = Ctrl_state.vy;
        packet1.vz = Ctrl_state.vz;

#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
	if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1)
	{
		// cope with extensions
		memset(MAVLINK_MSG_ID_RAW_IMU_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_RAW_IMU_MIN_LEN);
	}
#endif

	mavlink_msg_local_position_ned_send(MAVLINK_COMM_1 , packet1.time_boot_ms , packet1.x , packet1.y , packet1.z , packet1.vx , packet1.vy , packet1.vz );
}



void mavlink_test(void)
{
	static uint16_t test_count=0;
	mavlink_message_t lastmsg;
	test_count++;
	//5hz
	if((test_count%100)==0)
	{
		mavlink_test_heartbeat2(1,1,&lastmsg);
	}
	if((test_count%50)==0)
	{
		mavlink_test_raw_imu2(1,1,&lastmsg);
		mavlink_test_highres_imu(1,1,&lastmsg);
		mavlink_test_attitude(1,1,&lastmsg);
		mavlink_test_local_position_ned(1,1,&lastmsg);
	}
	
}
