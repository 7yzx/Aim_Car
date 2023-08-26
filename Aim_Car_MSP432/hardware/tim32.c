/****************************************************/
// 定时器32
/*
 * 定时器中断周期：
 *
 * T_timer_32 = CLKDIV * (ARR + 1) / f_clk 
 *            = 1 * 2400000 / 48000000 
 *            = 0.05s = 20Hz=50ms
 * 480000000 / 1 *240000=200hz=5ms
 * 48000000 / 1 * 480000=100hz=10ms
 */
/****************************************************/
#include "tim32.h"
#include "led.h"
#include "Encoder.h"
#include "Control.h"
#include "Sensor.h"
void Tim32_0_Int_Init(uint32_t aar, uint8_t psc)
{
    MAP_Timer32_initModule(TIMER32_0_BASE, psc, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Timer32_setCount(TIMER32_0_BASE, aar);
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false); //连续计数模式 false
    MAP_Interrupt_enableInterrupt(INT_T32_INT1);
}


/* Timer32 ISR */
void T32_INT1_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);
    if(Oled_go==1)
    {
      static int counto1=0;
      if(counto1>10)
      {
        counto1=0;
        Oled_go=0;
      }
    }
    GetEncoder();
		Get_Velocity_Form_Encoder(EncoderFL,EncoderFR,EncoderBL,EncoderBR);
    /*开始填充用户代码*/
		//tick_a2 0.5s
    Tick_a2();
    Tick_s1();
    if(Flag_start==1)
    {
      if(tick_s1==1||tick_s1==2 || tick_s2==1 ||tick_s2==2)
      {
        Flag_stop=0;
        Set_Pwm(800,800,800,800);
      }
      else
      {
        Sensor_Xunji_three();
      }
      Get_stopline();
      task_tackle(Stop_line);
      if(Flag_Adjust==0)//判断是否需要保持上一个状态   
      {
        if(Get_OpenMV_Stop==0)
        {
          if(Flag_stop==1)  //停止位是否置1
          {
            Set_Pwm(0,0,0,0);
          }
          else
          {
            Sensor_PID();
          }
        }
        else
        {
          Sensor_PID();
        }

      }	
    }

}

