/*
  ******************************************************************************
  * @file    scheduler.c
  * @author  across 
  * @version v1.0
  * @date    
  * @brief   ϵͳ״̬�ĸ��ºͿ������ĵ���
 ******************************************************************************
*/

#include "scheduler.h"
#include "rc_process.h"
#include "position_estimator_inav.h"
#include "sensor.h"
#include "md_struct.h"
#include "math.h"
#include "mc_att_control.h"
#include "mc_pos_control.h"
#include "md_twoboardcom.h"

uint8_t Sys_State = 0;
uint8_t State_Change_Prepared = 0;
uint16_t SystemStateUpdata_Count=0;
uint8_t disarming_flag = 0;
uint8_t armed_flag = 0;
uint16_t Mag_Static_Calibration_Count=0;
uint16_t SystemStateUpdata_Protection_Count=0;

float Acceptable_Error = 0.05f;


enum{
	ATTITUDE = 0,
	ALTITUDE,
	GPS,
};

uint8_t FlightMode = ATTITUDE;

uint8_t get_System_State(void)
{
	return Sys_State;
}
void set_System_State(uint8_t state)
{
	Sys_State = state;
}

/*
  * @brief  ϵͳ״̬����
  * @param  void
  * @note		ϵͳ״̬����PREPARING_STATE��STARTING_STATE��RUNNING_STATE����״̬���ɽ���״̬�����л�
  * @retval void
  * @author across
*/
void SystemStateUpdate(void) 
{
	
	switch (Sys_State)
	{
		case INIT_STATE:
			
			if((pRC_Normalization->Yaw < Nomalization_RC_Mid + Acceptable_Error && pRC_Normalization->Yaw > Nomalization_RC_Mid - Acceptable_Error)
					 &&(pRC_Normalization->Roll < Nomalization_RC_Mid + Acceptable_Error && pRC_Normalization->Roll > Nomalization_RC_Mid - Acceptable_Error)
						 &&(pRC_Normalization->Pitch < Nomalization_RC_Mid + Acceptable_Error && pRC_Normalization->Pitch > Nomalization_RC_Mid - Acceptable_Error)
							 &&(pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Throttle > Nomalization_RC_Min - Acceptable_Error)
								 && (pRC_Normalization->Switch_A == Nomalization_Switch_Min || pRC_Normalization->Switch_A==Nomalization_Switch_Max)
									 && pRC_Normalization->Switch_B == Nomalization_Switch_Min
										 && pRC_Normalization->Switch_C == Nomalization_Switch_Min
											 && pRC_Normalization->Switch_D == Nomalization_Switch_Min)
      {
        State_Change_Prepared = 1;
      }
			
			if ((pRC_Normalization->Yaw < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Yaw > Nomalization_RC_Min - Acceptable_Error)
          &&(pRC_Normalization->Pitch < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Pitch > Nomalization_RC_Min - Acceptable_Error)
            && (pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Throttle > Nomalization_RC_Min - Acceptable_Error)
              && (pRC_Normalization->Roll < Nomalization_RC_Max + Acceptable_Error && pRC_Normalization->Roll > Nomalization_RC_Max - Acceptable_Error)
                && (pRC_Normalization->Switch_A == Nomalization_Switch_Min || pRC_Normalization->Switch_A==Nomalization_Switch_Max)
									 && pRC_Normalization->Switch_B == Nomalization_Switch_Min
										 && pRC_Normalization->Switch_C == Nomalization_Switch_Min
											 && pRC_Normalization->Switch_D == Nomalization_Switch_Min)
      {
        SystemStateUpdata_Count++;
				Mag_Static_Calibration_Count=0;
        if(SystemStateUpdata_Count>=200 && State_Change_Prepared==1)
        {
          Sys_State = STANDBY_STATE;
          SystemStateUpdata_Count=0;
          State_Change_Prepared = 0;
					
        }
      }

      break;
				
		
		case STANDBY_STATE:
			
			if((pRC_Normalization->Yaw < Nomalization_RC_Mid + Acceptable_Error && pRC_Normalization->Yaw > Nomalization_RC_Mid - Acceptable_Error)
					 &&  (pRC_Normalization->Roll < Nomalization_RC_Mid + Acceptable_Error && pRC_Normalization->Roll > Nomalization_RC_Mid - Acceptable_Error)
						 &&  (pRC_Normalization->Pitch < Nomalization_RC_Mid + Acceptable_Error && pRC_Normalization->Pitch > Nomalization_RC_Mid - Acceptable_Error)
							 && (pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Throttle > Nomalization_RC_Min - Acceptable_Error))
      {
        State_Change_Prepared = 1;
      }
		
			if ((pRC_Normalization->Roll < Nomalization_RC_Mid + Acceptable_Error) && (pRC_Normalization->Roll > Nomalization_RC_Mid - Acceptable_Error)
          &&(pRC_Normalization->Pitch < Nomalization_RC_Mid + Acceptable_Error) && (pRC_Normalization->Pitch > Nomalization_RC_Mid - Acceptable_Error)
            &&(pRC_Normalization->Yaw < Nomalization_RC_Mid + Acceptable_Error) && (pRC_Normalization->Yaw > Nomalization_RC_Mid - Acceptable_Error)
              && (pRC_Normalization->Throttle > Nomalization_RC_Min + 5.0f*(Acceptable_Error))
                && (pRC_Normalization->Switch_A==Nomalization_Switch_Min || pRC_Normalization->Switch_A==Nomalization_Switch_Max)
                  && pRC_Normalization->Switch_B==Nomalization_Switch_Min
                    && pRC_Normalization->Switch_C==Nomalization_Switch_Min
                      && pRC_Normalization->Switch_D==Nomalization_Switch_Min
                        &&State_Change_Prepared==1)
      {
        Sys_State = RUNNING_STATE;
        SystemStateUpdata_Count=0;
        State_Change_Prepared=0;
      }
			
			if ((pRC_Normalization->Yaw < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Yaw > Nomalization_RC_Min - Acceptable_Error)
          &&(pRC_Normalization->Pitch < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Pitch > Nomalization_RC_Min - Acceptable_Error)
            && (pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Throttle > Nomalization_RC_Min - Acceptable_Error)
              && (pRC_Normalization->Roll < Nomalization_RC_Max + Acceptable_Error && pRC_Normalization->Roll > Nomalization_RC_Max - Acceptable_Error))
      {
        SystemStateUpdata_Count++;
        if((SystemStateUpdata_Count>=200&&State_Change_Prepared==1))
        {
          Sys_State = INIT_STATE;
          SystemStateUpdata_Count=0;
          State_Change_Prepared = 0;
        }
      }
      else
      {
        SystemStateUpdata_Count=0;
      }
		
			if ((pRC_Normalization->Roll < Nomalization_RC_Mid + 3.0f*Acceptable_Error) && (pRC_Normalization->Roll > Nomalization_RC_Mid - 3.0f*Acceptable_Error)
          &&(pRC_Normalization->Pitch < Nomalization_RC_Mid + 3.0f*Acceptable_Error) && (pRC_Normalization->Pitch > Nomalization_RC_Mid - 3.0f*Acceptable_Error)
            &&(pRC_Normalization->Yaw < Nomalization_RC_Mid + 3.0f*Acceptable_Error) && (pRC_Normalization->Yaw > Nomalization_RC_Mid - 3.0f*Acceptable_Error)
              &&(pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error))
      {
        SystemStateUpdata_Protection_Count++;
        if(SystemStateUpdata_Protection_Count >= 1200)
        {
          Sys_State = INIT_STATE;
          SystemStateUpdata_Protection_Count=0;
          SystemStateUpdata_Count=0;
          State_Change_Prepared=0;
        }
      }
      else
      {
        SystemStateUpdata_Protection_Count = 0;
      }
      break;
		
		case RUNNING_STATE:

			if ((pRC_Normalization->Yaw < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Yaw > Nomalization_RC_Min - Acceptable_Error)
          &&(pRC_Normalization->Pitch < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Pitch > Nomalization_RC_Min - Acceptable_Error)
            &&(pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error && pRC_Normalization->Throttle > Nomalization_RC_Min - Acceptable_Error)
              &&(pRC_Normalization->Roll < Nomalization_RC_Max + Acceptable_Error && pRC_Normalization->Roll > Nomalization_RC_Max - Acceptable_Error))
      {
        SystemStateUpdata_Count++;         
      }
      else
        SystemStateUpdata_Count = 0;
			
      if(SystemStateUpdata_Count > 100)
      {
        Sys_State = INIT_STATE;
        SystemStateUpdata_Count =0;
      }
      break;
			
		default:
      Sys_State = INIT_STATE;  
      break;	
	}
}

/*
  * @brief  �߼�����������
  * @param  void
  * @note		����ģʽ���У��ֳ����ǰ/�������׶�
  * @retval void
  * @author across
*/
void HighFlightmode_control_select(void)
{
	switch(FlightMode)
	{
		case ATTITUDE:
			
			Pos_Control_Init();
			Altitude_control_Init();
			Attitude_Reference_caculation();
			control_throttle = (pRC_Normalization->Throttle+1.0f)*672.0f;
		
		break;
		
		case GPS:
			if(Alt_State == Alt_Takeoff)
			{
				Alt_Control_Takeoff();
			}
			else if(Alt_State == Alt_Normal)
			{
				Takeoff_VelRef = 0.0f;
				Altitude_control_update(ALTCONTROL_MAX_THROTTLE, ALTCONTROL_MIN_THROTTLE);
				LandDetector();
				Pos_Control();
			}
		break;
		
	}
}

d_Motor Motor;
d_Motor *pMotor = &Motor;
uint16_t Control_Scheduler_Counter = 0;

/*
  * @brief  ����������
  * @param  void
  * @note		����ϵͳ״̬�����п������ĳ�ʼ�������У���ȫ���عرգ����������������
  * @retval void
  * @author across
*/
void Control_Scheduler(void)
{
	
	
	switch(Sys_State)
	{
		case INIT_STATE:
			
			Get_Altitude_Ground();
			Altitude_control_Init();
			Pos_Control_Init();
			mc_yaw_Init();
			mc_att_control_init();
			Alt_Control_Takeoff_Init();
		
			pMotor->M1 = MOTOR_MIN_POINT;
			pMotor->M2 = MOTOR_MIN_POINT;
			pMotor->M3 = MOTOR_MIN_POINT;
			pMotor->M4 = MOTOR_MIN_POINT;
			pMotor->M5 = MOTOR_MIN_POINT;
			pMotor->M6 = MOTOR_MIN_POINT;
			pMotor->M7 = MOTOR_MIN_POINT;
			pMotor->M8 = MOTOR_MIN_POINT;
			Com_TwoBoard_Msg_MotorValue_Send(pMotor);
		
			break;
			
		case STANDBY_STATE:
			
			Get_Altitude_Ground();
			Altitude_control_Init();
			Pos_Control_Init();
			mc_yaw_Init();
			mc_att_control_init();
			Alt_Control_Takeoff_Init();
			RCMODE_Update();
		
			pMotor->M1 = MOTOR_STARTING_POINT;
			pMotor->M2 = MOTOR_STARTING_POINT;
			pMotor->M3 = MOTOR_STARTING_POINT;
			pMotor->M4 = MOTOR_STARTING_POINT;
			pMotor->M5 = MOTOR_STARTING_POINT;
			pMotor->M6 = MOTOR_STARTING_POINT;
			pMotor->M7 = MOTOR_STARTING_POINT;
			pMotor->M8 = MOTOR_STARTING_POINT;
			Com_TwoBoard_Msg_MotorValue_Send(pMotor);

			break;
			
		case RUNNING_STATE:
			
			Control_Scheduler_Counter++;
			Control_Scheduler_Counter = Control_Scheduler_Counter >= 500 ? 0 : Control_Scheduler_Counter;
			
			RCMODE_Update();
		
			if(Control_Scheduler_Counter % 10 == 0)   //50hz ����
			{
				HighFlightmode_control_select();
			}
			
		
			mc_roll_pitch_control();
//			mc_yaw_control(0.002f);
			
			mc_rate_control();
			Ctr2Motor();

			break;
	}
}



/*******************************��������****************************************
* ��������: void RCMODE_Update(void)
* �������: void
* ���ز���: void
* ��    ��: ����ҡ�˿���A�����з���ģʽ���л�
* ��    ��: by across 
* ��    ��: 2020/02/27
*******************************************************************************/
void RCMODE_Update(void)
{
  switch ( pRC_Normalization->Switch_A )
  {
		case Nomalization_Switch_Min:
			if(Ctrl_state.xy_valid)
			{
					FlightMode = GPS;
			}
			else
				FlightMode = ATTITUDE;
			break;
				
		case Nomalization_Switch_Max:
			FlightMode = ATTITUDE;
			break;
		default: 
			break;
  };
}


uint16_t land_Cnt = 0;

static void LandDetector(void)
{
  if((pRC_Normalization->Throttle < Nomalization_RC_Min + Acceptable_Error)
     && (fabs(Ctrl_state.vz) < 0.7f) 
       && (sqrt(Ctrl_state.vx*Ctrl_state.vx+Ctrl_state.vy*Ctrl_state.vy) < 1.0f)
         && (fabs(Ctrl_state.gyrof.x)<0.3f && fabs(Ctrl_state.gyrof.y)<0.3f && fabs(Ctrl_state.gyrof.z) < 0.3f)
           && (control_throttle <= ALTCONTROL_MIN_THROTTLE*1.05f))
    land_Cnt++;
  else
    land_Cnt = 0;
	
  if(land_Cnt >=25)
  {
    land_Cnt = 0;
		
    Sys_State = INIT_STATE;
  }
}

uint16_t MagCail_Trig_Count = 0;
extern uint8_t Mag_Calibration_Instrction_Step;

void MagCail_Trig(void)
{
	static uint16_t MagCail_ProtectionTime_Count = 0;
	
	if(get_System_State() == INIT_STATE)
	{
		if(pRC_Normalization->Switch_B==Nomalization_Switch_Max && MagCail_Trig_Count%2==0 )
		{
			MagCail_Trig_Count++;
			MagCail_ProtectionTime_Count = 0;
		}
		else if(pRC_Normalization->Switch_B==Nomalization_Switch_Min && MagCail_Trig_Count%2==1)
		{
			MagCail_Trig_Count++;
			MagCail_ProtectionTime_Count = 0;
		}
		if(MagCail_Trig_Count>=8 && pRC_Normalization->Switch_B==Nomalization_Switch_Min)
		{
			MagCail_Trig_Count=0;
			Mag_Calibration_Instrction_Step = 1;
		}
		if(MagCail_ProtectionTime_Count++ >=1200)
		{
			MagCail_Trig_Count = 0;
			MagCail_ProtectionTime_Count = 1200;
		}
	}
	else
	{
		MagCail_Trig_Count = 0;
		MagCail_ProtectionTime_Count = 0;
		Mag_Calibration_Instrction_Step = 0;
	}
	
}


