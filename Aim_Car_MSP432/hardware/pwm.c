#include "pwm.h"

void PWM_motor_Init(void)
{
	  MAP_GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 + GPIO_PIN4+GPIO_PIN5+GPIO_PIN6 + GPIO_PIN7);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P10,GPIO_PIN1+GPIO_PIN0);
		GPIO_setOutputHighOnPin(GPIO_PORT_P10,GPIO_PIN0 );
		GPIO_setOutputHighOnPin(GPIO_PORT_P10,GPIO_PIN1 );

}