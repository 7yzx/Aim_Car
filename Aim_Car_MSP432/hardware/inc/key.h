/****************************************************/
//MSP432P401R
//板载按键驱动
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/8/11
/****************************************************/

#ifndef __KEY_H
#define __KEY_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define KEY1 BITBAND_PERI(P1IN, 1) //读取按键1
#define KEY2 BITBAND_PERI(P1IN, 4) //读取按键2

#define KEY1_PRES 1 //KEY1按下
#define KEY2_PRES 2 //KEY2按下


//IO初始化
void KEY_Init(bool mode);
 //按键扫描函数
uint8_t KEY_Scan(bool mode);
	/*
	mode1，三 1，     flag11为3
	mode2，两 1， 一2 flag11为2，flag22为1
	mode3，一 1， 两2 flag11为1，flag22为2
	mode4，三 2       flag11为0，flag22为3
	*/
void Key_Detect(void);

uint8_t click1(void);
uint8_t click2(void);

void test_start(void);
#endif
