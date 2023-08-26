#ifndef __PWM_H
#define __PWM_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define BIN1FL BITBAND_PERI(P7OUT,0)
#define BIN2FL BITBAND_PERI(P7OUT,2)
#define AIN1FR BITBAND_PERI(P7OUT,1)
#define AIN2FR BITBAND_PERI(P7OUT,3)

#define AIN1BR BITBAND_PERI(P7OUT,5)
#define AIN2BR BITBAND_PERI(P7OUT,7)
#define BIN1BL BITBAND_PERI(P7OUT,4)
#define BIN2BL BITBAND_PERI(P7OUT,6)
void PWM_motor_Init(void); 

#endif
