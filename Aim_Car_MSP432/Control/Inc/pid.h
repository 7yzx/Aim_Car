#ifndef _PID_H
#define _PID_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#define LIMIT(x,min,max) (x)=(((x)<=(min))?(min):(((x)>=(max))?(max):(x)))
#define ABS(x)		((x>0)? x: -x) 
typedef struct 
{
	float kp,ki,kd;
	int error,lastError;//误差、上次误差
  int derror;
	int integral,maxIntegral;//积分、积分限幅
	int output,maxOutput;//输出、输出限幅
}PID;
typedef enum 
{
	PID_Position,
	PID_Speed,
  PID_Sensor
}PID_ID;

/*PID初始化--------------------------------------------------------------*/
void PID_Init(PID* pid,float kp,float ki,float kd,int maxIntegral);
/*中途更改参数设定--------------------------------------------------------------*/
void PID_Reset(PID* pid,float kp,float ki,float kd);
/*pid计算-----------------------------------------------------------------------*/
int PID_Calc(PID* pid, int measure,int target);
#endif
