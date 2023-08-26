/****************************************************/
//MSP432P401R
//板载按键驱动
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/8/11
/****************************************************/

#include "key.h"
#include "Control.h"
//函数功能：延时
static void key_delay(uint16_t t);
uint8_t flag11,flag22;
//按键初始化函数
//mode:0,不开启中断;1,开启中断
void KEY_Init(bool mode) //IO初始化
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 + GPIO_PIN4);

    if (mode)
    {
        /* Configuring P1.1 P1.4 as an input and enabling interrupts */
        MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1); //清理中断标志
        MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4); //清理中断标志
        MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);    //使能中断端口
        MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);    //使能中断端口
        MAP_Interrupt_enableInterrupt(INT_PORT1);             //端口总使能

        /* Enabling SRAM Bank Retention */
        MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);
    }
}

/* GPIO 中断 */
void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取中断状态
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);         //清理中断标志
    key_delay(25);                                             //去抖动

    if (status & GPIO_PIN1) //对应P1.1
    {
        if (KEY1 == 0)
        {
            /*开始填充用户代码*/

            /*结束填充用户代码*/
        }
    }
    if (status & GPIO_PIN4) //对应P1.4
    {
        if (KEY2 == 0)
        {
            /*开始填充用户代码*/

            /*结束填充用户代码*/
        }
    }
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//注意此函数有响应优先级,KEY1>KEY2!!
uint8_t KEY_Scan(bool mode)
{
    static bool key_up = true; //按键按松开标志
    if (mode)
        key_up = true; //支持连按
    if (key_up && (KEY1 == 0 || KEY2 == 0))
    {
        key_delay(25); //去抖动
        key_up = false;
        if (KEY1 == 0)
            return KEY1_PRES;
        else if (KEY2 == 0)
            return KEY2_PRES;
    }
    else if (KEY1 == 1 && KEY2 == 1)
        key_up = true;
    return 0; // 无按键按下
}

uint8_t click1(void)
{
	//按键按松开标志
	static bool flag_key=true;
	//按键按下
	if(flag_key&&KEY1==0)
	{
        key_delay(25); //去抖动
		flag_key=0;
        if (KEY1 == 0)
            return 1;
	}
	//无按键按下
	else if(1==KEY1)			
		flag_key=true;
	return 0;
}
uint8_t click2(void)
{
	//按键按松开标志
	static bool flag_key2=true;
	//按键按下
	if(flag_key2&&KEY2==0)
	{
        key_delay(50); //去抖动
		flag_key2=true;
        if (KEY2 == 0)
            return 1;
	}
	//无按键按下
	else if(1==KEY2)			
		flag_key2=true;
		return 0;
}

void Key_Detect(void)
{
	uint8_t tmp1,tmp2;
	tmp1 = click1();
	tmp2 = click2();

	if(tmp1==1)
	{
		flag11++;
		if(flag11==5)
		{
			flag11=1;
		}
	}
	if(tmp2==1)
	{
		flag22++;
		if(flag22==5)
		{
			flag22=1;
		}
	}

	if(flag11==4)
	{
		Mode=1;
	}

	else if((flag11==3) && (flag22==1))
	{
        Mode=2;
	}

	else if((flag11==2)&&(flag22==2))
	{
        Mode=3;

	}
	else if((flag11==1) && (flag22==3))
	{
        Mode=4;
	}
    else if((flag11==0)&& (flag22==4))
    {
        Mode=5;
    }
}

void test_start(void)
{
    static uint8_t test1=0;
    if(Mode==5)
    {
        if(click1()==1)
        {
            test1++;
        }
        if(test1==1)
        {
            Get_OpenMV_Stop=1;
            test1++;
            flag_s1=1;
        }
        if(test1==3)
        {
            Get_OpenMV_Stop=1;
            test1++;
            flag_s2=1;

        }
        if(test1==5)
        {
            test1++;
        }
    }


}
//函数功能：延时
void key_delay(uint16_t t)
{
    volatile uint16_t x;
    while (t--)
        for (x = 0; x < 1000; x++)
            ;
}
