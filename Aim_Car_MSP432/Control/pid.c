#include "pid.h"
void PID_Init(PID* pid,float kp,float ki,float kd,int maxIntegral)
{
	pid->kp=kp;
	pid->ki=ki;
	pid->kd=kd;
	pid->maxIntegral=maxIntegral;
	pid->maxOutput=4500;
}
void PID_Reset(PID* pid,float kp,float ki,float kd)
{
	pid->kp=kp;
	pid->ki=ki;
	pid->kd=kd;
}

int PID_Calc(PID* pid, int measure,int target)
{
	pid->error=target-measure;
	pid->integral+=pid->error;
	pid->derror = pid->error - pid->lastError;
	//限幅
	if(pid->integral > pid->maxIntegral)
	{
		pid->integral=pid->maxIntegral;
	}
	if(pid->integral <  -pid->maxIntegral)
	{
		pid->integral= -pid->maxIntegral;
	}
	pid->lastError=pid->error;
	pid->output=pid->kp * pid->error + pid->ki * pid->integral + pid->kd * pid->derror;

	if(pid->output>pid->maxOutput)         
	{
		pid->output = pid->maxOutput;
	}
	if(pid->output < -(pid->maxOutput))
	{
		pid->output = -(pid->maxOutput);
	}
	return pid->output;
}
