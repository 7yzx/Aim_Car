
#ifndef __CONTROL_H
#define __CONTROL_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define PI 3.14159265							//PI

#define PWMFL   TA0CCR2 						//PWMA1
#define PWMFR   TA0CCR1  						//PWMA2
#define PWMBL   TA0CCR3 						//PWMB1
#define PWMBR   TA0CCR4 						//PWMB2


#define Control_Frequency  100.0					
#define Diameter_67        67.0 						//67mm 
#define EncoderMultiples   4.0 					//
#define Encoder_precision  13.0 				//
#define Reduction_Ratio    30.0					//

#define CCRN_MIN 499  							// (499 + 1) / (19999 + 1) =  500 / 20000 =  2.5%
#define CCRN_MAX 2499 							// (2499 + 1) / (19999 + 1) = 2500 / 20000 = 12.5%
extern int Mode;

extern int EncoderCntFL,EncoderCntFR,EncoderCntBL,EncoderCntBR;
extern int EncoderFL,EncoderFR,EncoderBL,EncoderBR;
extern int Velocity_FL,Velocity_FR,Velocity_BL,Velocity_BR;
extern int Motor_Front_Left,Motor_Front_Right,Motor_Behind_Left,Motor_Behind_Right;
extern int Velocity_PwmFL,Velocity_PwmFR,Velocity_PwmBL,Velocity_PwmBR;            //四轮pwm量

extern int Flag_start;
extern int Flag_stop;
extern int Flag_turnangle;
extern int turn_a2,tick_a2;
extern int Flag_turncircle;
extern int Stop_line_num,Stop_line,flag_stopline,flag_stoplinelast;

extern uint8_t OpenMV_Recognize;

extern uint8_t Get_OpenMV_Find;
extern uint8_t Get_OpenMV_Move;
extern uint8_t Get_OpenMV_thing;
extern uint8_t Get_OpenMV_Stop;
extern uint8_t OpenMV_Send;

extern uint8_t Flag_Move;
extern int Flag_Move_num1;
extern int Flag_Move_num2;
extern uint8_t OPENMV_SendArray[6];

extern uint8_t flag11,flag22;
extern int flag_s1,flag_s2,flag_s3;
extern int tick_s1,tick_s2,tick_s3;
extern int flag_get1,flag_get2,flag_get3;
extern int Oled_go;
extern int stop_count1,stop_count2,stop_count3;

/**************************************************************************
函数功能：转角计时
使用参数：turn_a2
得到的数据：tick_a2
**************************************************************************/
void Tick_a2(void);
/**************************************************************************
函数功能：停止位启动计时屏蔽停止位
使用参数：flag_s1
得到的数据：tick_s1
**************************************************************************/
void Tick_s1(void);
void Tick_s2(void);
/**************************************************************************
函数功能：得到经过stopline的次数
使用参数：Stop_line_num，flag_stopline，flag_stoplinelast
得到的数据：Stop_line
**************************************************************************/
void Get_stopline(void);

void OpenMV_Move(void);

void Set_4Pwm(int pwm);
void Set_Pwm(int motor_FL,int motor_FR,int motor_BL,int motor_BR);
int PWM_Limit(int IN,int max,int min);
void Get_Velocity_Form_Encoder(int encoder_fl,int encoder_fr,int encoder_bl,int encoder_br);
//可以修改status，任务四的时候，status==5
void task_tackle(int stopline);
uint32_t myabs(long int a);
#endif
