/****************************************************/

#include "sysinit.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"

#include "delay.h"
#include "led.h"
#include "tim32.h"
#include "Control.h"
#include "Show.h"
#include "Encoder.h"
#include "pwm.h"
#include "pid.h"
#include "TimA0.h"
#include "Sensor.h"
#include "key.h"
#include "oled.h"

#define CLKDIV TIMER32_PRESCALER_1 // 时钟源分频
#define ARR 480000-1               // 自动重装载值

int Mode;
int EncoderCntFL=5000,EncoderCntFR=5000,EncoderCntBL=5000,EncoderCntBR=5000;
int EncoderFL,EncoderFR,EncoderBL,EncoderBR;
//电机PWM变量
int Motor_Front_Left,Motor_Front_Right,Motor_Behind_Left,Motor_Behind_Right;
int Velocity_PwmFL,Velocity_PwmFR,Velocity_PwmBL,Velocity_PwmBR;//四轮pwm量
int Velocity_FL,Velocity_FR,Velocity_BL,Velocity_BR;


int Flag_start;                                             //开始标志位
int Flag_stop;                                              //停止标志位
int Flag_turnangle;                                         //转角标志位                                        
int turn_a2,tick_a2;                                        //转角计时 
int Flag_turncircle;                                        //弯道标志位 

int Stop_line;                                              //经过的总数
int Stop_line_num;                                          //计时位
int flag_stopline,flag_stoplinelast;                        //标志位，得到经过的总数的

int Flag_Move_num1;
int Flag_Move_num2;

//openmv
uint8_t OPENMV_SendArray[6];                               // 发送数组,0:2C,1:12,5:5B2:识别位, 1  3: 任务位   4:
                                                          //
uint8_t OpenMV_Send;
uint8_t OpenMV_Recognize;                                  //openmv开始识别
uint8_t Get_OpenMV_Find;                                   //从openmv中得到是否识别找到标志
uint8_t Get_OpenmV_Start;                                  //从OpenMV中得到是否开始     
uint8_t Get_OpenMV_Move;
uint8_t Get_OpenMV_thing;
uint8_t Get_OpenMV_Stop;
uint8_t Flag_Move;

int flag_s1,flag_s2,flag_s3;
int tick_s1,tick_s2,tick_s3;
int flag_get1,flag_get2,flag_get3;
int Oled_go=0;

int stop_count1,stop_count2,stop_count3;

int main(void)
{
                                                            //初始化
  SysInit();         																				// 时钟配置
  KEY_Init(0);
  Tim32_0_Int_Init(ARR, CLKDIV); 														//TIM32中断
  uart_init(115200);                                        // 串口配置
  uart2_init(115200);
  uart3_init(115200);

  delay_init();                                             // 滴答延时
  PWM_motor_Init();                                         //电机pwm初始化
	Sensor_init();                                            //传感器初始化
  LED_Init();                                               //LED初始化
  OLED_Init();
	Encoder_Init();                                           //编码器中断初始化
	TimA0_PWM_Init(4800, TIMER_A_CLOCKSOURCE_DIVIDER_1);			// 10K
	MAP_Interrupt_enableMaster();                             // 开启总中断


	while (1)
	{
    OLED_ShowOpenMV();
		Key_Detect();		                                         //扫描按键状态 	
		LED_RED_Tog();												                   // LED翻转，程序运行
		delay_ms(100);
		Usart_show();
		switch (Mode)
    {
      case 1:
      {
        //Flag_start=1;//进入直接启动了定时器，但是在黑线上所以不会动，并且告诉openmv去识别
        if(Flag_stop==1)
        {
          LED_Stop();
          Set_Pwm(0,0,0,0);
        }
				Serial_SendByte(0x10);//发送出去，一直发送


      }
        break;
      case 2:
      {
				//Flag_start=1;//进入直接启动了定时器，但是在黑线上所以不会动，并且告诉openmv去识别
        if(Flag_stop==1)
        {
          LED_Stop();
          Set_Pwm(0,0,0,0);
        }
				Serial_SendByte(0x20);//发送出去，一直发送
      }
        break;
      case 3:
      {
				Flag_start=1;
        if(Get_OpenMV_Stop==0)
        {
          if(Flag_stop==1)
          {
            Set_Pwm(0,0,0,0);
            LED_Stop();												                // LED红灯，车子停止
          }
          else
          {
            OpenMV_Send=0x30;
          }
        }
				Serial_SendByte(0x30);//发送出去，一直发送
      }
        break;
      case 4:
      {
        Flag_start=1;//进入直接启动了定时器，但是在黑线上所以不会动，并且告诉openmv去识别
        if(Get_OpenMV_Stop==0)
        {
          if(Flag_stop==1)
          {
            Set_Pwm(0,0,0,0);
            LED_Stop();												                // LED红灯，车子停止
          }
          else
          {
            OpenMV_Send=0x40;
            LED_B_On();
          }
        }
        else
        {
          OpenMV_Send=0x40;
        }
				Serial_SendByte(OpenMV_Send);//发送出去，一直发送
      }
      case 5:
      {
        Flag_start=1;//进入直接启动了定时器，但是在黑线上所以不会动，并且告诉openmv去识别

        if(Get_OpenMV_Stop==0)
        {
          if(Flag_stop==1)
          {
            Set_Pwm(0,0,0,0);
            LED_Stop();												                // LED红灯，车子停止
          }
          else
          {
            OpenMV_Send=0x50;
            LED_B_On();
          }
        }
        else
        {
          OpenMV_Send=0x50;
          LED_B_On();
        }
				Serial_SendByte(OpenMV_Send);//发送出去，一直发送
      }
        break;
      default:
        break;
    }
	}
}
