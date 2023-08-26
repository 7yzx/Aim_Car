#include "Sensor.h"
#include "pid.h"
#include "usart.h"
#include "Control.h"
int LL1,RR1,LL2,RR2,MM;
int flag_;
int Flag_Adjust;
int sensor_bias;
int status;
PID Sen_PID;
PID Turn_PID;
void Sensor_init(void)
{
	GPIO_setAsInputPin(GPIO_PORT_P9,GPIO_PIN1);	//L2	
	GPIO_setAsInputPin(GPIO_PORT_P8,GPIO_PIN7);  //L1
	GPIO_setAsInputPin(GPIO_PORT_P8,GPIO_PIN2);  //M
	GPIO_setAsInputPin(GPIO_PORT_P8,GPIO_PIN4);  //R1
	GPIO_setAsInputPin(GPIO_PORT_P9,GPIO_PIN0);  //R2
}

void Sensor_Xunji_three(void)
{
	//如果没有检测到，就会保持之前的值，继续PI
	//00000
	Flag_Adjust=0;
	if(((L2)!=BIT1) && ((L1)!=BIT7) && ((M)!=BIT2) && ((R1)!=BIT4)&& ((R2)!=BIT0)) 
	{
		Sensor_PID();
		Flag_Adjust=1;
	}
	//黑线
  else 	if(((L2)==BIT1) && ((L1)==BIT7) && ((M)==BIT2) && ((R1)==BIT4)&& ((R2)==BIT0)) 
	{
		flag_=1;
		switch (Mode)
		{
			case 1:
			{
				Flag_stop=1;
				status=10;
			}
				break;
			case 2:
			{
				Flag_stop=1;
				status=10;
			}
				break;
			case 3:
			{
				Flag_stop=1;
				status=10;
			}
				break;
			case 4:
			{
				Stop_line_num++;
				status=1;
			}
			case 5:
			{
				//任务五 起点黑线，停止了  stopnum++
				Flag_stop=1;
				Stop_line_num++;
				flag_=10;
				if(tick_s1==1||tick_s1==2|| tick_s2==1 || tick_s2==2 || tick_s3==1 || tick_s3==2 )
				{
					Flag_stop=0;
					Stop_line_num=0;
				}
			}
				break;
			default:Flag_stop=1;
				break;
		}
	}
	// 00100
  else 	if(((L2)!=BIT1) && ((L1)!=BIT7) && ((M)==BIT2) && ((R1)!=BIT4)&& ((R2)!=BIT0)) 
	{
		sensor_bias = 0;
		status = 3;
	}
	//右偏了，01000
  else if(((L2)!=BIT1) && ((L1)==BIT7) && ((M)!=BIT2) && ((R1)!=BIT4)&& ((R2)!=BIT0)) 
	{
		flag_=3;
		sensor_bias = 30;
		status = 3;

	}
//左偏，00010
  else 	if(((L2)!=BIT1) && ((L1)!=BIT7) && ((M)!=BIT2) && ((R1)==BIT4)&& ((R2)!=BIT0)) 
	{
		flag_=4;
		sensor_bias = -30;
		status = 3;
	}
  else 	if(((L2)!=BIT1) && ((L1)!=BIT7) && ((M)!=BIT2) && ((R1)!=BIT4)&& ((R2)==BIT0)) 
	{
		sensor_bias = -40;
		status = 3;
	}
	//10000
  else 	if(((L2)==BIT1) && ((L1)!=BIT7) && ((M)!=BIT2) && ((R1)!=BIT4)&& ((R2)!=BIT0)) 
	{
		flag_=5;
	}
	//遇到直角
  else 	if((((L2)!=BIT1) && ((L1)!=BIT7) && ((M)==BIT2) && ((R1)==BIT4)&& ((R2)==BIT0))
					|| (((L2)!=BIT1) && ((L1)!=BIT7) && ((M)!=BIT2) && ((R1)==BIT4)&& ((R2)==BIT0)))
	{
		if(Mode==2 || Mode==4 ||Mode ==5)
		{
			turn_a2=1;
			Set_Pwm(0,0,0,0);
			status=7;
		}
	}
	else
	{
		sensor_bias=0;
	}
}

void Sensor_PID()
{
	static uint8_t flag_stopturn=0;
	//直角转弯，0.5-1s
	if(tick_a2>=1 && tick_a2<25 && flag_stopturn==0)
	{
		Set_Pwm(580,-510,580,-510);	
  	if(((L2)!=BIT1) && ((L1)!=BIT7) && ((M)==BIT2) && ((R1)!=BIT4)&& ((R2)!=BIT0)) 
		{
			flag_stopturn=1;
		}
		status=7;
	}
	if(tick_a2==25 || (flag_stopturn==1 && tick_a2<25))
	{
		Set_Pwm(0,0,0,0);	
		status=1;
	}

	if(status==6)
	{
		//直接调用，当前值，目标
		PID_Calc(&Sen_PID,sensor_bias,0);
		Set_4Pwm(800);
		Motor_Front_Left=(Velocity_PwmFL + Sen_PID.output);
		Motor_Front_Right=(Velocity_PwmFR - Sen_PID.output);
		Motor_Behind_Left=(Velocity_PwmBL + Sen_PID.output*(1.3));
		Motor_Behind_Right=(Velocity_PwmBR - Sen_PID.output*(1.3));
	  Set_Pwm(Motor_Front_Left,Motor_Front_Right,Motor_Behind_Left,Motor_Behind_Right);	
	}
	else if(status==5)
	{
		PID_Init(&Turn_PID,20.5,0.05,0.11,1000);
		PID_Calc(&Turn_PID,sensor_bias,0);
		Set_4Pwm(800);
		Motor_Front_Left=(Velocity_PwmFL + Turn_PID.output*(1.3));
		Motor_Front_Right=(Velocity_PwmFR - Turn_PID.output);
		Motor_Behind_Left=(Velocity_PwmBL + Turn_PID.output*(1.3))*1.3;
		Motor_Behind_Right=(Velocity_PwmBR - Turn_PID.output)*1.3;
	  Set_Pwm(Motor_Front_Left,Motor_Front_Right,Motor_Behind_Left,Motor_Behind_Right);	
	}
	else if(status<=4)
	{
		//初始化Sen_PID，kp，ki，kd，积分限幅
		PID_Init(&Sen_PID,23.5,0.01,0.01,1000);
		//直接调用，当前值，目标
		PID_Calc(&Sen_PID,sensor_bias,0);
		Set_4Pwm(800);
		Motor_Front_Left=(Velocity_PwmFL + Sen_PID.output);
		Motor_Front_Right=(Velocity_PwmFR - Sen_PID.output);
		Motor_Behind_Left=(Velocity_PwmBL + Sen_PID.output*(1.3));
		Motor_Behind_Right=(Velocity_PwmBR - Sen_PID.output*(1.3));
	  Set_Pwm(Motor_Front_Left,Motor_Front_Right,Motor_Behind_Left,Motor_Behind_Right);	
	}
	else if(status==10)
	{
		Set_Pwm(0,0,0,0);
	}
}
