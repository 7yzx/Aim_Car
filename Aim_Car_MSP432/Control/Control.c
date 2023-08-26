#include "Control.h"
#include "pwm.h"
#include "Sensor.h"
#include "usart3.h"

#define LIMIT(x,min,max) (x)=(((x)<=(min))?(min):(((x)>=(max))?(max):(x)))

void Tick_a2(void)
{
	if(turn_a2==1)
	{
		static int count2 = 0;
		if (count2 > 50)
		{
			count2 = 0;
			tick_a2++;
		}
		count2++;
	}
}

void Tick_s1(void)
{
	if(flag_s1==1)
	{
		static int counts1 = 0;
		if (counts1 > 50)
		{
			counts1 = 0;
			tick_s1++;
		}
		counts1++;
	}
	if(flag_s2==1)
	{
		static int counts2 = 0;
		if (counts2 > 40)
		{
			counts2 = 0;
			tick_s2++;
		}
		counts2++;
	}
	if(flag_s3==1)
	{
		static int counts3 = 0;
		if (counts3 > 50)
		{
			counts3 = 0;
			tick_s3++;
		}
		counts3++;
	}
	//限制
  if(tick_a2>200)
  {
    turn_a2=0;
  }
  if(tick_s1>200)
  {
    flag_s1=0;
  }
  if(tick_s2>200)
  {
    flag_s2=0;
  }
  if(tick_s3>200)
  {
    flag_s3=0;
  }



}

void Get_stopline(void)
{
	if(Stop_line_num>=1)
	{
		flag_stopline=1;
		flag_stoplinelast=flag_stopline;
	}
	if(flag_stopline==0 && flag_stoplinelast==1)
	{
		Stop_line++;
		flag_stoplinelast=0;
	}
	Stop_line_num=0;
	flag_stopline=0;

}

void task_tackle(int stopline)
{

	if(Mode==3)
	{
		status=5;
	}
	if(Mode==4)
	{
		static int flag_in1=0;
		if(Get_OpenMV_Stop==1)
		{
			Get_OpenMV_Move=1;
		}
		
		if(Get_OpenMV_Move==1)
		{
			flag_get1=1;
			Get_OpenMV_Stop=0;
			OpenMV_Send=0x50;
			Get_OpenMV_Move=0;
		}
		if(flag_get1==1)// 第一次启动
		{
			flag_s1=1;   //启动标志
			flag_in1=1;//不再进入这个
			Get_OpenMV_Stop=0;
		}
		
		if(stopline==0)
		{
		}
		if(stopline==1)
		{
		}
		else if(stopline==2)
		{
			status=5;
		}
		else if(stopline>=3)
		{
			Flag_stop=1;
		}
	}
	if(Mode==5)
	{
		static int flag_s=0;
		static int flag_in1=0;
		static int flag_in2=0;
		//test_start();
		if(stopline==1)
		{
			if(Flag_stop==1 && stop_count2<20)
			{
				OpenMV_Send=0x51;
				stop_count2++;
			}
			else
			{
				OpenMV_Send=0x50;
			}
			//status=6;					//减速
		}
		else if(stopline==2)
		{
			if(Flag_stop==1 && stop_count3<30)
			{
				OpenMV_Send=0x51;
				stop_count3++;
			}
			else
			{
				OpenMV_Send=0x50;
			}
			status=5;
		}

		if(Get_OpenMV_Stop==1)
		{
			Get_OpenMV_Move=1;
		}
		
		if(Get_OpenMV_Move==1)
		{
			flag_get1=1;
			Get_OpenMV_Stop=0;
			flag_s++;//记录转变的次数
			OpenMV_Send=0x50;
			Get_OpenMV_Move=0;
			if(flag_in1==1 && tick_s1>=10)
			{
				flag_get2=1;
			}
			if(flag_in2==1 && tick_s2>=10)
			{
				flag_get3=1;
			}
		}
		if(flag_get1==1)// 第一次启动
		{
			flag_s1=1;   //启动标志
			flag_in1=1;//不再进入这个
			Get_OpenMV_Stop=0;
		}
		if(flag_get2==1)// 第二次启动
		{
			flag_s2=1;
			flag_in2=1;
		}
		if(flag_get3==1)// 第三次启动
		{
			flag_s3=1;
		}
		//判断是否要进入弯道
		if(stopline==3)
		{
			status=5;
		}
	}
}
//电机转速  转速=编码器读数（10ms每次）*读取频率/倍频数/减速比/编码器精度
void Get_Velocity_Form_Encoder(int encoder_fl,int encoder_fr,int encoder_bl,int encoder_br)
{ 	
	float Rotation_Speed_FL,Rotation_Speed_FR,Rotation_Speed_BL,Rotation_Speed_BR;				
	Rotation_Speed_FL = encoder_fl*Control_Frequency/EncoderMultiples/Reduction_Ratio/10/Encoder_precision;
	Velocity_FL = (float)Rotation_Speed_FL*PI*Diameter_67;		//求出编码器速度=转速*周长
	Rotation_Speed_FR = encoder_fr*Control_Frequency/EncoderMultiples/Reduction_Ratio/10/Encoder_precision;
	Velocity_FR = (float)Rotation_Speed_FR*PI*Diameter_67;		//求出编码器速度=转速*周长
	
	Rotation_Speed_BL = encoder_bl*Control_Frequency/EncoderMultiples/Reduction_Ratio/10/Encoder_precision;
	Velocity_BL = (float)Rotation_Speed_BL*PI*Diameter_67;		//求出编码器速度=转速*周长
	Rotation_Speed_BR = encoder_br*Control_Frequency/EncoderMultiples/Reduction_Ratio/10/Encoder_precision;
	Velocity_BR = (float)Rotation_Speed_BR*PI*Diameter_67;		//求出编码器速度=转速*周长
}

void Set_4Pwm(int pwm)
{
	Velocity_PwmFL=pwm;        
	Velocity_PwmFR=pwm;        
	Velocity_PwmBL=pwm;        
	Velocity_PwmBR=pwm;
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_FL,int motor_FR,int motor_BL,int motor_BR)
{
	if(motor_FL==0 && motor_FR==0 && motor_BL==0 && motor_BR==0)
	{
		BIN1FL=0;BIN2FL=0;
		AIN1FR=0;AIN2FR=0;
		BIN1BL=0;BIN2BL=0;
		AIN1BR=0;AIN2BR=0;
	}
	if(motor_FL>0)
	{
		BIN1FL=0,BIN2FL=1;
	}
	else
	{
		BIN1FL=1,BIN2FL=0;
	}
	PWMFL=myabs(motor_FL);
	PWMFL=PWM_Limit(PWMFL,7000,-7000);
	
	if(motor_FR>0)
	{
		AIN1FR=0,AIN2FR=1;
	}
	else
	{
		AIN1FR=1,AIN2FR=0;
	}
	PWMFR=myabs(motor_FR);
	PWMFR=PWM_Limit(PWMFR,7000,-7000);

	if(motor_BL>0)
	{
		BIN1BL=0,BIN2BL=1;
	}
	else
	{
		BIN1BL=1,BIN2BL=0;
	}
	PWMBL=myabs(motor_BL);
	PWMBL=PWM_Limit(PWMBL,7000,-7000);

	if(motor_BR>0)
	{
		AIN1BR=0,AIN2BR=1;
	}
	else
	{
		AIN1BR=1,AIN2BR=0;
	}
	PWMBR=myabs(motor_BR);
	PWMBR=PWM_Limit(PWMBR,7000,-7000);
}

int PWM_Limit(int IN,int max,int min)
{
	int OUT = IN;
	if(OUT>max) 
		OUT = max;
	if(OUT<min) 
		OUT = min;
	return OUT;
}

uint32_t myabs(long int a)
{ 		   
	uint32_t temp;
	if(a<0)  
		temp=-a;  
	else 
		temp=a;
	return temp;
}
