/****************************************************/
// MSP432P401R
// 串口配置
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
/****************************************************/

/******************   版本更新说明   *****************
 * 
 * CCS支持printf
 * Keil支持标准C库跟微库
 * 用Keil开发终于可以不开微库啦
 * 
 * ? 需要注意：
 * ①使用标准C库时，将无法使用scanf。
 * 如果需要使用scanf时，请使用微库 MicroLIB
 * ①低频时钟频率下，高波特率使得传输时误差过大,
 * 比如35768Hz下19200波特率,
 * 会使得传输出错，这时可以尝试降低波特率。
 * ②baudrate_calculate的问题请去文件内查看。
 * 
 ****************************************************/

#include "usart.h"
#include "baudrate_calculate.h"
uint8_t UART0_TX_BUF[UART0_SEND_LEN_MAX]; //发送缓冲,最大UART2_SEND_LEN_MAX字节

#ifdef __TI_COMPILER_VERSION__
//CCS平台
uint8_t  USART0_TX_BUF[USART0_MAX_SEND_LEN];             //发送缓冲,最大USART3_MAX_SEND_LEN字节
int printf(const char *str, ...)
{
    uint16_t i,j;
    va_list ap;
    va_start(ap,str);
    vsprintf((char*)USART0_TX_BUF,str,ap);
    va_end(ap);
    i=strlen((const char*)USART0_TX_BUF);       //此次发送数据的长度
    for(j=0;j<i;j++)                            //循环发送数据
    {
      //while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        UART_transmitData(EUSCI_A0_BASE, USART0_TX_BUF[j]);
    }
    return 0;
}
/*****************   函数说明   *****************
 *
 * 函数：int printf(const char *str, ...);
 * 源码来自@正点原子
 * 稍作改动适配CCS工程，在此也表感谢正点原子。
 *
 *****************   说明结束   *****************/

#else
//Keil支持标准C库跟微库
//预编译
//if 1 使用标准C库 如果报错就使用微库
//if 0 使用微库 得去勾选魔术棒里的 Use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
  int handle;
};
FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
  x = x;
}
#else
int fgetc(FILE *f)
{
  while (EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG !=
         UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
    ;
  return UART_receiveData(EUSCI_A0_BASE);
}
#endif
int fputc(int ch, FILE *f)
{
  UART_transmitData(EUSCI_A0_BASE, ch & 0xFF);
  return ch;
}
/*****************   函数说明   *****************
 *
 * 以上两条对接标准输入输出库的函数:
 * int fputc(int ch, FILE *f);
 * int fgetc(FILE *f);
 * 源码为BiliBili平台UP主 “CloudBoyStudio” 编写
 * 本人RNA，不是作者
 * 在此也表感谢
 *
 *****************   说明结束   *****************/
#endif

void uart_init(uint32_t baudRate)
{
#ifdef EUSCI_A_UART_7_BIT_LEN
  //固件库v3_40_01_02
  //默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_ConfigV1 uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
          EUSCI_A_UART_8_BIT_LEN                         // 8 bit data length
      };
  eusci_calcBaudDividers((eUSCI_UART_ConfigV1 *)&uartConfig, baudRate); //配置波特率
#else
  //固件库v3_21_00_05
  //默认SMCLK 48MHz 比特率 115200
  const eUSCI_UART_Config uartConfig =
      {
          EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
          26,                                            // BRDIV = 26
          0,                                             // UCxBRF = 0
          111,                                           // UCxBRS = 111
          EUSCI_A_UART_NO_PARITY,                        // No Parity
          EUSCI_A_UART_LSB_FIRST,                        // MSB First
          EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
          EUSCI_A_UART_MODE,                             // UART mode
          EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
      };
  eusci_calcBaudDividers((eUSCI_UART_Config *)&uartConfig, baudRate); //配置波特率
#endif

  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
  MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
  MAP_UART_enableModule(EUSCI_A0_BASE);
  UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
  Interrupt_enableInterrupt(INT_EUSCIA0);
  Interrupt_enableMaster();
}
//串口0,printf 函数
void printf_uart0(char *fmt, ...)
{
	uint16_t i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)UART0_TX_BUF, fmt, ap);
	va_end(ap);
	i = strlen((const char *)UART0_TX_BUF); //此次发送数据的长度
	for (j = 0; j < i; j++)					//循环发送数据
	{
		MAP_UART_transmitData(EUSCI_A0_BASE, UART0_TX_BUF[j]);
	}
}
//串口0中断处理函数
//void EUSCIA0_IRQHandler(void)
//{
//    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

//    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //接收中断
//    {
//        UART_transmitData(EUSCI_A0_BASE, MAP_UART_receiveData(EUSCI_A0_BASE)); //发送数据
//    }

//}
