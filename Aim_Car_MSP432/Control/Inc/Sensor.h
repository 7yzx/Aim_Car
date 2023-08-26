#ifndef _SENSOR_H
#define _SENSOR_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "pid.h"
#define R2 P9IN & BIT0
#define R1 P8IN & BIT4
#define M  P8IN & BIT2
#define L1 P8IN & BIT7
#define L2 P9IN & BIT1
extern int Flag_Adjust;
extern int LL1,RR1,LL2,RR2,MM;
extern int sensor_bias;
extern int status;
extern PID Sen_PID;
extern PID Turn_PID;
extern int flag_;
void Sensor_init(void);
void Sensor_PID(void);

/*status,小于4 都是正常的循迹；
          5直角
          6弯道
          7减速
          10停止
	sensor_bias ，通过灰度传感器得到的偏差，自己设定和pi控制有着比较大的关联
  Flag_stop,    123任务停止
  Stop_line_num 4任务记次数

*/
void Sensor_Xunji_three(void);

#endif
