#include "usart3.h"
#include "Control.h"
#include "baudrate_calculate.h"
uint8_t UART3_TX_BUF[UART3_SEND_LEN_MAX]; //发送缓冲,最大UART2_SEND_LEN_MAX字节

void uart3_init(uint32_t baudRate)
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

  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P9, GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
  MAP_UART_initModule(EUSCI_A3_BASE, &uartConfig);
  MAP_UART_enableModule(EUSCI_A3_BASE);
  UART_enableInterrupt(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
  Interrupt_enableInterrupt(INT_EUSCIA3);
  Interrupt_enableMaster();
}

void printf_uart3(char *fmt, ...)
{
	uint16_t i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)UART3_TX_BUF, fmt, ap);
	va_end(ap);
	i = strlen((const char *)UART3_TX_BUF); //此次发送数据的长度
	for (j = 0; j < i; j++)					//循环发送数据
	{
		MAP_UART_transmitData(EUSCI_A3_BASE, UART3_TX_BUF[j]);
	}
}

//openmv数据接受中断
void EUSCIA3_IRQHandler(void)
{
	uint8_t com_data; 							//接收一个字节数据的临时变量
	uint8_t i;
	static uint8_t RxCounter3=0;				//数据缓冲区的索引
	static uint8_t RxBuffer3[6]={0};			//存放数据的接收缓存区
	static uint8_t RxState = 0;					//接收标志位
	uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A3_BASE);
	if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //接收中断
	{
		com_data = MAP_UART_receiveData(EUSCI_A3_BASE); // 临时接收
		if(RxState==0&&com_data==0x2C)  		//0x2c帧头
		{
			RxState=1;
			RxBuffer3[RxCounter3++]=com_data;	//RxBuffer1[0]==0x2c RxCounter1==12
		}
		else if(RxState==1&&com_data==0x12)  	//0x12帧头
		{
			RxState=2;
			RxBuffer3[RxCounter3++]=com_data;	//RxBuffer1[0]==0x2c RxCounter1==12
		}
		else if(RxState==2)						//开始接收有效数据
		{
			RxBuffer3[RxCounter3++]=com_data;	//全部接收完，RxCounter1==5

			if(RxCounter3>=6 || com_data == 0x5B)       //RxBuffer1接受满了,接收数据结束
			{
				RxState=3;
			}
		}
		else if(RxState==3)						//检测是否接受到结束标志
		{
			if(RxBuffer3[RxCounter3-1] == 0x5B)
			{
				UART_disableInterrupt(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);								
				RxCounter3 = 0;
				RxState = 0;
				//在此处理数据
        Receive_Data(RxBuffer3);
				//用户填入代码
				UART_enableInterrupt(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
			}
			else   //接收错误
			{
				RxState = 0;
				RxCounter3=0;
				for(i=0;i<6;i++)
				{
					RxBuffer3[i]=0xff;      //将存放数据数组清零
				}
			}
		} 
		else   //接收异常
		{
			RxState = 0;
			RxCounter3=0;
			for(i=0;i<6;i++)
			{
				RxBuffer3[i]=0xff;      //将存放数据数组清零
			}
		}
	}
}

//openmv数据接受处理函数
void Receive_Data(uint8_t *data_buf)
{
	if(data_buf[0]==0x2C && data_buf[1]==0x12 && data_buf[5]==0x5B)
	{
		Get_OpenMV_thing=data_buf[2];     
    Flag_start=data_buf[3];
		Get_OpenMV_Stop=data_buf[4];
	}
}

void Serial_SendByte(uint8_t data)
{
	MAP_UART_transmitData(EUSCI_A3_BASE, data);
	while (UART_getInterruptStatus(EUSCI_A3_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == 0);
}

//void Serial_SendArray(uint8_t *Array)
//{
//	Array[0]=0x2C;
//	Array[1]=0x12;
//	Array[5]=0x5B;
//	for (int i = 0; i < 6; i ++)
//	{
//		Serial_SendByte(Array[i]);
//	}
//}




