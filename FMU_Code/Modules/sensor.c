/*
  ******************************************************************************
  * @file    sensor.c
  * @author  across 
  * @version v1.0
  * @date    2020-02-27
  * @brief   ���������ݶ�ȡ��У�����˲�����
 ******************************************************************************
*/
#include "sensor.h"
#include "filter.h"
#include "md_mpu6000.h"
#include "md_ms56xx.h"
#include "Mag_calibration.h"

extern MPU6000_Data 		m_Mpu6000;
extern hmc5883MagData 	m_Hmc5883;
float pitch_rate_filtered,roll_rate_filtered,yaw_rate_filtered;
extern float attitude_roll,attitude_pitch,attitude_yaw;

static uint8_t flag_calib_falied = 1;
static int16_t gyro_last[3] = {0};
static uint16_t gyro_int[3] = {0};
static int16_t gyro_offset_x = 0,gyro_offset_y = 0,gyro_offset_z = 0;

static uint16_t Sensor_1000HZ_Timer_Counter = 0;
float BARO_Alt = 0.0f;

/*ע�����ٶȼƺ͵شżƵ�У�����ֶ�У�������޳���У������ͬ�ķɻ���������������*/
static float acc_offset_x = -0.05f,acc_offset_y = -0.16f,acc_offset_z = -0.7f;

//float magxoffset = 0.24f;
//float magyoffset = 0.05f;
//float magzoffset = 0.33f;


float mag_max[3];
float mag_min[3];
uint8_t flag_clibration_mag = 0;
float dt_mag_cal = 0.05f;

//���Ʒ�������
Control_state  Ctrl_state;

//IIR�˲���������������
IIR_coeff_Typedef Filter_Param_Gx;
IIR_coeff_Typedef *pFilter_Param_Gx = &Filter_Param_Gx;
IIR_coeff_Typedef Filter_Param_Gy;
IIR_coeff_Typedef *pFilter_Param_Gy = &Filter_Param_Gy;
IIR_coeff_Typedef Filter_Param_Gz;
IIR_coeff_Typedef *pFilter_Param_Gz = &Filter_Param_Gz;
IIR_coeff_Typedef Filter_Param_Ax;
IIR_coeff_Typedef *pFilter_Param_Ax = &Filter_Param_Ax;
IIR_coeff_Typedef Filter_Param_Ay;
IIR_coeff_Typedef *pFilter_Param_Ay = &Filter_Param_Ay;
IIR_coeff_Typedef Filter_Param_Az;
IIR_coeff_Typedef *pFilter_Param_Az = &Filter_Param_Az;


/*******************************��������****************************************
* ��������: void Filter_init(void)
* �������: void
* ���ز���: void
* ��    ��: IIR�˲�����ʼ��  �����˲�������
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/ 
void Filter_init(void)
{
	cal_iir_coeff(pFilter_Param_Gx, IMU_Sample_Hz, GYRO_CutoffFreq);
  cal_iir_coeff(pFilter_Param_Gy, IMU_Sample_Hz, GYRO_CutoffFreq);
  cal_iir_coeff(pFilter_Param_Gz, IMU_Sample_Hz, GYRO_CutoffFreq);
	
	cal_iir_coeff(pFilter_Param_Ax, IMU_Sample_Hz, ACC_CutoffFreq);
  cal_iir_coeff(pFilter_Param_Ay, IMU_Sample_Hz, ACC_CutoffFreq);
  cal_iir_coeff(pFilter_Param_Az, IMU_Sample_Hz, ACC_CutoffFreq);
}


/*******************************��������****************************************
* ��������: void Transform(float Aframex,float Aframey,float Aframez,float *Bframex,float *Bframey,float *Bframez)
* �������: float Aframex,float Aframey,float Aframez,float *Bframex,float *Bframey,float *Bframez
* ���ز���: void
* ��    ��: �����趨������ϵ��ת�����ݵ�������
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/ 
void Transform(float Aframex,float Aframey,float Aframez,float *Bframex,float *Bframey,float *Bframez)
{
    *Bframex = Aframex;
    *Bframey = Aframey;
    *Bframez = Aframez;
}

/*******************************��������****************************************
* ��������: void IMU_Data_Combine(void)
* �������: void
* ���ز���: void
* ��    ��: ͳһ���������ݵĵ�λ��������ƫ�Լ������˲�������30hz
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/ 
void IMU_Data_Combine(void)
{
	float gyro_range_scale = (0.0174532f / 16.4f); //����2000��/��  ��ԭʼ����ת���ɻ���/��
	float accel_range_scale = MPU6000_ONE_G * (1.0f / 8192.0f); //����4g
	
	//��mpuԭʼ����ת���ɱ�׼��λ  
	m_Mpu6000.gyrof.x = (m_Mpu6000.gyro_raw.x - gyro_offset_x) * gyro_range_scale;
	m_Mpu6000.gyrof.y = (m_Mpu6000.gyro_raw.y - gyro_offset_y) * gyro_range_scale;
	m_Mpu6000.gyrof.z = (m_Mpu6000.gyro_raw.z - gyro_offset_z) * gyro_range_scale;
	
	m_Mpu6000.accf.x = m_Mpu6000.acc_raw.x * accel_range_scale - acc_offset_x;
	m_Mpu6000.accf.y = m_Mpu6000.acc_raw.y * accel_range_scale - acc_offset_y;
	m_Mpu6000.accf.z = m_Mpu6000.acc_raw.z * accel_range_scale - acc_offset_z;
	
	//ת�����ݵ�����ϵ
	//����ϵ  ��ǰ��  xyz
	Transform(m_Mpu6000.gyrof.y,-m_Mpu6000.gyrof.x,m_Mpu6000.gyrof.z,&m_Mpu6000.gyrof.x,&m_Mpu6000.gyrof.y,&m_Mpu6000.gyrof.z);
	Transform(m_Mpu6000.accf.y,-m_Mpu6000.accf.x,m_Mpu6000.accf.z,&m_Mpu6000.accf.x,&m_Mpu6000.accf.y,&m_Mpu6000.accf.z);
	
	//����IIR��ͨ�˲���
	Ctrl_state.gyrof.x = get_iir_output(pFilter_Param_Gx, m_Mpu6000.gyrof.x);
  Ctrl_state.gyrof.y = -get_iir_output(pFilter_Param_Gy, m_Mpu6000.gyrof.y);
  Ctrl_state.gyrof.z = -get_iir_output(pFilter_Param_Gz, m_Mpu6000.gyrof.z);
	
	Ctrl_state.accf.x = get_iir_output(pFilter_Param_Ax, m_Mpu6000.accf.x);
  Ctrl_state.accf.y = get_iir_output(pFilter_Param_Ay, m_Mpu6000.accf.y);
  Ctrl_state.accf.z = get_iir_output(pFilter_Param_Az, m_Mpu6000.accf.z);
	
	//�ش����ݸ�ֵ  ��λ ��˹
	Ctrl_state.magf.x = -m_Hmc5883.y;  
	Ctrl_state.magf.y =  m_Hmc5883.x;
	Ctrl_state.magf.z =  m_Hmc5883.z;
	
	Sensor_1000HZ_Timer_Counter++;
	Sensor_1000HZ_Timer_Counter = Sensor_1000HZ_Timer_Counter >= 1000 ? 0 : Sensor_1000HZ_Timer_Counter;
			
	if(Sensor_1000HZ_Timer_Counter % 10 == 0)   //50hz ����
	{
		BARO_Alt = Get_Baro_Alt(m_Ms56xx.pressure);
	}
	
	pitch_rate_filtered = (1.0f - 0.4f) * pitch_rate_filtered + 0.4f * Ctrl_state.gyrof.y;
	roll_rate_filtered  = (1.0f - 0.4f) * roll_rate_filtered  + 0.4f * Ctrl_state.gyrof.x;
	yaw_rate_filtered   = (1.0f - 0.4f) * yaw_rate_filtered   + 0.4f * Ctrl_state.gyrof.z;
	
	Ctrl_state.roll  =  attitude_roll;
	Ctrl_state.pitch = -attitude_pitch;
	Ctrl_state.yaw   = -attitude_yaw;
	
}


/*******************************��������****************************************
* ��������: float Get_Baro_Alt(float P)
* �������: ��ѹ
* ���ز���: ���θ߶�
* ��    ��: ����ѹת���ɸ߶�
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/ 
float Get_Baro_Alt(float P)
{
	float alt;

	alt = 44330.77f * (1 - powf((P / 101325),0.190263f));
	
	return alt;
}


/*******************************��������****************************************
* ��������: void Get_Gyro_Offset(void)
* �������: void
* ���ز���: void
* ��    ��: ��ȡ�����ǵ����ݣ���ʼ�����ã�gyro���У׼
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/ 
void Get_Gyro_Offset(void)
{
  static int32_t offset_sumx = 0, offset_sumy = 0, offset_sumz = 0;
  uint8_t k = 0;
  
  while(flag_calib_falied)
  {
    if(k < 200)
    {
			MPU6000_Get_Data(&m_Mpu6000);
			HAL_Delay(20);
      
      if(k == 0)
      {
        gyro_last[0] = m_Mpu6000.gyro_raw.x;
        gyro_last[1] = m_Mpu6000.gyro_raw.y;
        gyro_last[2] = m_Mpu6000.gyro_raw.z;  
      }
      
      gyro_int[0] += ABS(m_Mpu6000.gyro_raw.x - gyro_last[0]);
      gyro_int[1] += ABS(m_Mpu6000.gyro_raw.y - gyro_last[1]);
      gyro_int[2] += ABS(m_Mpu6000.gyro_raw.z - gyro_last[2]);
      
      offset_sumx += m_Mpu6000.gyro_raw.x;
      offset_sumy += m_Mpu6000.gyro_raw.y;
      offset_sumz += m_Mpu6000.gyro_raw.z;
      
      k++;
    }
    else
    {
			
      if((gyro_int[0] > 500) || (gyro_int[1] > 500) || (gyro_int[2] > 500))
      {
        flag_calib_falied = 1;
        k = 0;
        offset_sumx = 0;
        offset_sumy = 0;
        offset_sumz = 0;
        for(uint8_t j = 0; j < 3; j++)
        {
          gyro_int[j] = 0;
        }
      }
      else
      {
        flag_calib_falied = 0;
      }
    }
  }
  
  gyro_offset_x  = (offset_sumx/200);
  gyro_offset_y  = (offset_sumy/200);
  gyro_offset_z  = (offset_sumz/200);
}





/*
 * mag calibration
 */
void mag_calibration(void)
{
//	static uint8_t flag_clean = 1;
//	static float x = 0, z = 0;
//	
//	if(flag_clean) {
//		x = z = 0;
//		for(uint8_t i = 0; i < 3; i++){
//			mag_max[i] = 0;
//			mag_min[i] = 0;
//		}

//		flag_clean = 0;
//	}
//	//find min and max
//	if(mag.x > mag_max[0]){
//		mag_max[0] = mag.x;
//	}else if(mag.x < mag_min[0]){
//		mag_min[0] = mag.x;
//	}	
//	if(mag.y > mag_max[1]){
//		mag_max[1] = mag.y;
//	}else if(mag.y < mag_min[1]){
//		mag_min[1] = mag.y;
//	}
//	if(mag.z > mag_max[2]){
//		mag_max[2] = mag.z;
//	}else if(mag.z < mag_min[2]){
//		mag_min[2] = mag.z;
//	}
//	
//	
//	switch(flag_clibration_mag)
//	{
//		case 1:
//				if(accfilter.z > 6.0f){
//		
//					z += gyrofilter.z * dt_mag_cal;

//					if (fabsf(z) > 6.28f) {
//						flag_clibration_mag = 2;

//					}
//				}
//			break;
//		
//		case 2:
//				if(flag_clibration_mag == 2 && accfilter.x < -6.0f){
//		
//					x += gyrofilter.x * dt_mag_cal;

//					if (fabsf(x) > 6.28f) {
//						magxoffset = (mag_max[0] + mag_min[0]) * 0.5f;
//						magyoffset = (mag_max[1] + mag_min[1]) * 0.5f;
//						magzoffset = (mag_max[2] + mag_min[2]) * 0.5f;
//						flag_clibration_mag = 3;
//						flag_clean = 1;
//					}
//				}
//			break;
//		
//		case 3:
//					
//			break;
//		
//		default:
//				flag_clibration_mag = 1;
//			break;
//	}
	
	
	
}


