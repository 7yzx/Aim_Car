
#include "timA0.h"

/*********************************************************************************************************/

/**************************************         TIMA0          *******************************************/
//编码器初始化函数
void TimA0_PWM_Init(uint16_t ccr0, uint16_t psc)
{
    /*初始化引脚*/
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION); //通道1
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION); //通道2
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION); //通道3
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); //通道4

    const Timer_A_PWMConfig TimA0_PWMConfig = 
	{
        TIMER_A_CLOCKSOURCE_SMCLK,         		//时钟源
        psc,                                 	//时钟分频 范围1-64
        ccr0,                                 	//自动重装载值（ARR）
        TIMER_A_CAPTURECOMPARE_REGISTER_1, 		//通道1 （注意引脚定义）
        TIMER_A_OUTPUTMODE_RESET_SET,     		//输出模式7，一旦到CCRx值，就复位为低，到CCR0就置位为高
        0                               		//这里是改变占空比的地方 默认100%
    };
    Timer_A_PWMConfig *User_TimA0_PWMConfig = (Timer_A_PWMConfig*)&TimA0_PWMConfig;
    /*定时器PWM初始化*/
    User_TimA0_PWMConfig->clockSourceDivider = psc;
    User_TimA0_PWMConfig->timerPeriod = ccr0;
    //第1路 PWM
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig); /* 初始化比较寄存器以产生 PWM1 */

    //第2路 PWM
    User_TimA0_PWMConfig->compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2; //通道2 （注意引脚定义）
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig);                  /* 初始化比较寄存器以产生 PWM2 */

    //第3路 PWM
    User_TimA0_PWMConfig->compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3; //通道3 （注意引脚定义）
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig);                  /* 初始化比较寄存器以产生 PWM3 */

    //第4路 PWM
    User_TimA0_PWMConfig->compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4; //通道4 （注意引脚定义）
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig);                  /* 初始化比较寄存器以产生 PWM4 */
}

