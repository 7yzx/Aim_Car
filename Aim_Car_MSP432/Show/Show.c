#include "Show.h"
#include "Control.h"
#include "usart.h"
#include "Sensor.h"
#include "pid.h"
#include "key.h"
void Usart_show()
{
	printf_uart0("Stop_line%d\r\n",Stop_line);
	printf_uart0("Flag_stop:%d\r\n",Flag_stop);
  printf_uart0("tick_s1%d\r\n",tick_s1);
	printf_uart0("flag_s1:%d\r\n",flag_s1);
  printf_uart0("flag_s2:%d\r\n",flag_s2);
  printf_uart0("flag_s3:%d\r\n",flag_s3);

	printf_uart0("flag_:%d\r\n",flag_);
  printf_uart0("tick_s2%d\r\n",tick_s2);
	// printf_uart0("MM:%d\r\n",MM);
  // printf_uart0("RR1:%d\r\n",RR1);
	// printf_uart0("RR2:%d\r\n",RR2);
}
