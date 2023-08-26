#ifndef __USART3_H
#define __USART3_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h> //1.61328125kb
#include "string.h"
#include "stdarg.h"
#define UART3_SEND_LEN_MAX 256 						// uart2 最大发送缓存字节数

void uart3_init(uint32_t baudRate);

void printf_uart3(char *fmt, ...);


//openmv接收数据处理
void Receive_Data(uint8_t *data_buf);
//msp432 发送一 byte
void Serial_SendByte(uint8_t data);
//msp432 发送数组
void Serial_SendArray(uint8_t *Array);
//msp432 发送数据处理
void Send_Data(uint8_t *data);
#endif
