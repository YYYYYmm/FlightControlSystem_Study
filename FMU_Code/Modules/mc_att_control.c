/*******************************************************************************
* �ļ����ƣ�mc_att_control.c
* ժ    Ҫ����̬������������PID���⻷�Ƕ��ڻ����ٶȣ�
* ��ǰ�汾��v1.0
* ��    �ߣ�across 
* ��    �ڣ�2020/02/27
* ���뻷����keil5
* ��ʷ��Ϣ��
*******************************************************************************/


#include "mc_att_control.h"
#include "math.h"
#include "sensor.h"
#include "rc_process.h"
#include "attitude_estimator_mahony.h"
#include "position_estimator_inav.h"
#include "md_twoboardcom.h"
#include "mc_pos_control.h"


float dt_control = 0.002f;
float pitchratesp,rollratesp,yawratesp;
float pitch_rate_iterm,roll_rate_iterm,yaw_rate_iterm;
float pitch_rate_derivative,roll_rate_derivative,yaw_rate_derivative;

float last_pitch_rate,last_roll_rate ,last_yaw_rate;


float pitch_rate_kp,roll_rate_kp,yaw_rate_kp;
float pitch_rate_ki,roll_rate_ki,yaw_rate_ki;
float pitch_rate_kd,roll_rate_kd,yaw_rate_kd;

float control_pitch,control_roll,control_yaw,control_throttle;
float YAWCONTROLLER_Angle_Pre = 0.0f, YAWCONTROLLER_Compensate = 0.0f, YAWCONTROLLER_AttitudeControl_Feedback=0.0f,YAWCONTROLLER_Initial_Mag_Heading = 0.0f;

float pitch_kp,roll_kp,yaw_kp;

float KStickDirect_T = 0.65f;

extern d_Motor *pMotor;


/*******************************��������****************************************
* ��������: void mc_rate_control(void)
* �������: void
* ���ز���: void
* ��    ��: ���ٶ�PID����
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void mc_rate_control(void)
{
	float e_rate_pitch = 0.0f, e_rate_roll = 0.0f, e_rate_yaw = 0.0f;
	float theta = 0.0f;
  theta = fabs(cos(attitude_roll)*cos(attitude_pitch));
  theta = theta < 0.7f ? 0.7f : theta;

  control_throttle = control_throttle*((1.0f/theta - 1.0f)/2.0f + 1.0f);
  control_throttle = control_throttle < 0.0f ? 0.0f : control_throttle;
  control_throttle = control_throttle > ALTCONTROL_MAX_THROTTLE ? ALTCONTROL_MAX_THROTTLE : control_throttle;    
  
	yawratesp = pRC_Normalization->Yaw * 672.0f;
	
	e_rate_pitch = -pitchratesp * 0.005f + Ctrl_state.gyrof.y;
	e_rate_roll  =  rollratesp  * 0.005f - Ctrl_state.gyrof.x;
	e_rate_yaw   = -yawratesp   * 0.005f - Ctrl_state.gyrof.z;
	
	if(control_throttle > ALTCONTROL_MIN_THROTTLE)
	{	
		pitch_rate_iterm += pitch_rate_ki * e_rate_pitch * dt_control;
		roll_rate_iterm  += roll_rate_ki  * e_rate_roll  * dt_control;
		yaw_rate_iterm   += yaw_rate_ki   * e_rate_yaw   * dt_control;
	}
	else
	{
		roll_rate_iterm  = 0.0f;
		pitch_rate_iterm = 0.0f;
		yaw_rate_iterm   = 0.0f;
	}
	
	pitch_rate_iterm = CONSTRAINT(pitch_rate_iterm, -300.0f, 300.0f);
	roll_rate_iterm  = CONSTRAINT(roll_rate_iterm,  -300.0f, 300.0f);
	yaw_rate_iterm   = CONSTRAINT(yaw_rate_iterm,   -300.0f, 300.0f);
	
	
	
	pitch_rate_derivative = -(pitch_rate_filtered - last_pitch_rate) / dt_control * pitch_rate_kd;
	roll_rate_derivative  =  (roll_rate_filtered  - last_roll_rate)  / dt_control * roll_rate_kd;
	yaw_rate_derivative   = -(yaw_rate_filtered   - last_yaw_rate)   / dt_control * yaw_rate_kd;
	
	last_pitch_rate = pitch_rate_filtered;
	last_roll_rate  = roll_rate_filtered;
	last_yaw_rate   = yaw_rate_filtered;
	
	control_pitch = pitch_rate_kp * e_rate_pitch + pitch_rate_iterm + pitch_rate_derivative;
	control_roll  = roll_rate_kp  * e_rate_roll  + roll_rate_iterm  + roll_rate_derivative;
	control_yaw   = yaw_rate_kp   * e_rate_yaw   + yaw_rate_iterm   + yaw_rate_derivative;
}


/*******************************��������****************************************
* ��������: void Attitude_Reference_caculation(void)
* �������: void
* ���ز���: void
* ��    ��: ��̬�ǿ��� Ŀ��ֵ���㣬�ɸ���ֱ��ӳ�䵽�Ƕ�Ŀ��
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void Attitude_Reference_caculation(void)
{
  rollsp     = pRC_Normalization->Roll  * (AttiLimit * DEG2RAD);
  pitchsp    = pRC_Normalization->Pitch * (AttiLimit * DEG2RAD);
}


/*******************************��������****************************************
* ��������: void mc_roll_pitch_control(void)
* �������: void
* ���ز���: void
* ��    ��: ��̬�ǿ��� ����P
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void mc_roll_pitch_control(void)
{
	
	pitchratesp = pitch_kp * (pitchsp - Ctrl_state.pitch);
	rollratesp  = roll_kp  * (rollsp  - Ctrl_state.roll);
	
	pitchratesp = CONSTRAINT(pitchratesp, -600.0f, 600.0f);
	rollratesp  = CONSTRAINT(rollratesp,  -600.0f, 600.0f);
}


/*******************************��������****************************************
* ��������: void Yaw_Reference_Calculation(float dt)
* �������: ��������
* ���ز���: void
* ��    ��: yawĿ��ֵ����
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void Yaw_Reference_Calculation(float dt)
{
	float frequency = 1.0f / dt;
	if(pRC_Normalization->Yaw > 0.09f)
	{
		yawsp -=  (RefYawrate*DEG2RAD)*(pRC_Normalization->Yaw - 0.09f)/(1.0f - 0.09f)/frequency;
	}
	else if(pRC_Normalization->Yaw < -0.09f)
	{
		yawsp -=  (RefYawrate*DEG2RAD)*(pRC_Normalization->Yaw + 0.09f)/(1.0f - 0.09f)/frequency;
	}
      
	
	if(attitude_yaw - YAWCONTROLLER_Angle_Pre > 5.0f)
	{
			YAWCONTROLLER_Compensate = YAWCONTROLLER_Compensate - M_TWOPI_F;
	}
	else if(attitude_yaw - YAWCONTROLLER_Angle_Pre < -5.0f)
	{
			YAWCONTROLLER_Compensate = YAWCONTROLLER_Compensate + M_TWOPI_F;
	}
	
	YAWCONTROLLER_AttitudeControl_Feedback =  Ctrl_state.yaw + YAWCONTROLLER_Compensate;
	YAWCONTROLLER_Angle_Pre = Ctrl_state.yaw;
}

/*******************************��������****************************************
* ��������: void mc_yaw_control(float dt)
* �������: ��������
* ���ز���: void
* ��    ��: yaw�Ƕȿ���
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void mc_yaw_control(float dt)
{
	Yaw_Reference_Calculation(dt);
	yawratesp = (YAWCONTROLLER_AttitudeControl_Feedback - yawsp) * yaw_kp;
	yawratesp = CONSTRAINT(yawratesp, -1000.0f, 1000.0f);
}

/*******************************��������****************************************
* ��������: void mc_yaw_Init(void)
* �������: void
* ���ز���: void
* ��    ��: yaw�Ƕȿ��Ƴ�ʼ��  ѡȡyaw���
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void mc_yaw_Init(void)
{
    yawsp = Ctrl_state.yaw;
    YAWCONTROLLER_Compensate = 0.0f;
    YAWCONTROLLER_Angle_Pre = Ctrl_state.yaw;
}

/*******************************��������****************************************
* ��������: void mc_att_control_init(void)
* �������: void
* ���ز���: void
* ��    ��: ��̬��������ʼ��  ������ֵ ��������
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void mc_att_control_init(void)
{
	pitch_kp = 600.0f;
	roll_kp  = 600.0f;
	yaw_kp   = 1500.0f;
	
	pitch_rate_kp = 80.0f;  
	roll_rate_kp  = 80.0f; 
	yaw_rate_kp   = 350.0f;
	
	pitch_rate_ki = 200.0f; 
	roll_rate_ki  = 200.0f;
	yaw_rate_ki   = 250.0f;
	
	pitch_rate_kd = 2.0f;
	roll_rate_kd  = 2.0f;
	yaw_rate_kd   = 0.0f;

	pitch_rate_derivative = 0.0f;
	roll_rate_derivative  = 0.0f;
	yaw_rate_derivative   = 0.0f;
	
	last_pitch_rate = pitch_rate_filtered;
	last_roll_rate  = roll_rate_filtered;
	last_yaw_rate   = yaw_rate_filtered;
	
	reset_integrator();
}


/*******************************��������****************************************
* ��������: void Ctr2Motor(void)
* �������: void
* ���ز���: void
* ��    ��: ������ת���ɵ��PWM
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void Ctr2Motor(void)
{
	pMotor->M1	=(int16_t)((((control_throttle * KStickDirect_T ) + control_roll + control_pitch + control_yaw)) + MOTOR_STARTING_POINT);
  pMotor->M2	=(int16_t)((((control_throttle * KStickDirect_T ) - control_roll + control_pitch - control_yaw)) + MOTOR_STARTING_POINT);
  pMotor->M3	=(int16_t)((((control_throttle * KStickDirect_T ) - control_roll - control_pitch + control_yaw)) + MOTOR_STARTING_POINT);
  pMotor->M4	=(int16_t)((((control_throttle * KStickDirect_T ) + control_roll - control_pitch - control_yaw)) + MOTOR_STARTING_POINT);
	
	pMotor->M1 = CONSTRAINT(pMotor->M1,1000,2000);
	pMotor->M2 = CONSTRAINT(pMotor->M2,1000,2000);
	pMotor->M3 = CONSTRAINT(pMotor->M3,1000,2000);
	pMotor->M4 = CONSTRAINT(pMotor->M4,1000,2000);
	Com_TwoBoard_Msg_MotorValue_Send(pMotor);
}





/*******************************��������****************************************
* ��������: void reset_integrator(void)
* �������: void
* ���ز���: void
* ��    ��: ��������������
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void reset_integrator(void)
{
	roll_rate_iterm  = 0.0f;
	pitch_rate_iterm = 0.0f;
	yaw_rate_iterm   = 0.0f;
}


