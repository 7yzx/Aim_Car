#include "Encoder.h"
#include "Control.h"

void Encoder_Init(void) 
{
	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3,GPIO_PIN3|GPIO_PIN7);
	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5,GPIO_PIN0|GPIO_PIN1|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

	GPIO_clearInterruptFlag(GPIO_PORT_P3,GPIO_PIN3|GPIO_PIN7);
	GPIO_clearInterruptFlag(GPIO_PORT_P4,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
	GPIO_clearInterruptFlag(GPIO_PORT_P5,GPIO_PIN0|GPIO_PIN1|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
	//下降沿
	GPIO_interruptEdgeSelect(GPIO_PORT_P3,GPIO_PIN5,GPIO_HIGH_TO_LOW_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P4,GPIO_PIN0|GPIO_PIN1|GPIO_PIN4|GPIO_PIN6,GPIO_HIGH_TO_LOW_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P5,GPIO_PIN0|GPIO_PIN4|GPIO_PIN6,GPIO_HIGH_TO_LOW_TRANSITION);
	//上升沿
	GPIO_interruptEdgeSelect(GPIO_PORT_P3,GPIO_PIN7,GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P4,GPIO_PIN2|GPIO_PIN3|GPIO_PIN5|GPIO_PIN7,GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P5,GPIO_PIN1|GPIO_PIN5|GPIO_PIN7,GPIO_LOW_TO_HIGH_TRANSITION);
	
	GPIO_enableInterrupt(GPIO_PORT_P3,GPIO_PIN3|GPIO_PIN7);
	GPIO_enableInterrupt(GPIO_PORT_P4,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
	GPIO_enableInterrupt(GPIO_PORT_P5,GPIO_PIN0|GPIO_PIN1|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
	
	Interrupt_enableInterrupt(INT_PORT3);
	Interrupt_enableInterrupt(INT_PORT4);
	Interrupt_enableInterrupt(INT_PORT5);
	Interrupt_enableMaster();
}
void GetEncoder(void)
{
	EncoderFL=EncoderCntFL-5000;                 // 读取编码器的值
	EncoderFR=5000-EncoderCntFR;                  // 读取编码器的值
	EncoderBL=EncoderCntBL-5000;
	EncoderBR=5000-EncoderCntBR;
	EncoderCntFL=5000;                               // 置位
	EncoderCntFR=5000;                               // 置位
	EncoderCntBL=5000;  
	EncoderCntBR=5000;  
}
//
void PORT3_IRQHandler(void)
{
    uint16_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3); 	//
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);         	//
	//p3.5 BR B相下降沿
	if(status & GPIO_PIN5) 										
	{
		if(P5IN&(1<<4))//
      EncoderCntBR--;//
		else
			EncoderCntBR++;//
	}
	//p3.7  BR B相上升沿
	else if(status & GPIO_PIN7)
	{
		if(P5IN&(1<<5))
      EncoderCntBR++;
		else
			EncoderCntBR--;
	}
}

void PORT4_IRQHandler(void)
{
	uint16_t status;
  status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P4); 	//
  MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4, status);         	//
	//p4.0 FL A相下降沿
	if(status & GPIO_PIN0) 										
	{
		if(P4IN&(1<<1))//
      EncoderCntFL++;//
		else
			EncoderCntFL--;//
	}
	//p4.1 FL B相下降沿
	else if(status & GPIO_PIN1)
	{
		if(P4IN&(1<<0))
      EncoderCntFL--;
		else
			EncoderCntFL++;
	}
	//p4.4 FR A相下降沿
	else if(status & GPIO_PIN4)
	{
		if(P4IN&(1<<6))//
      EncoderCntFR++;//
		else
			EncoderCntFR--;//
	}
	//p4.6 FR B相下降沿
	else if(status & GPIO_PIN6)
	{
		if(P4IN&(1<<4))
      EncoderCntFR--;
		else
			EncoderCntFR++;//
	}
	//p4.2 FL A相上升沿
	else if(status & GPIO_PIN2)
	{
		if(P4IN&(1<<3))
      EncoderCntFL--;//
		else
			EncoderCntFL++;//
	}
	//p4.3 FL B相上升沿
	else if(status & GPIO_PIN3)
	{
		if(P4IN&(1<<2))//
      EncoderCntFL++;//
		else
			EncoderCntFL--;//
	}
	//p4.5 FR A相上升沿
	else if(status & GPIO_PIN5)
	{
		if(P4IN&(1<<7))//
      EncoderCntFR--;//
		else
			EncoderCntFR++;//
	}
	//p4.7 FR B相上升沿
	else if(status & GPIO_PIN7)
	{
		if(P4IN&(1<<5))//
      EncoderCntFR++;//
		else
			EncoderCntFR--;//
	}

}
void PORT5_IRQHandler(void)
{
	uint16_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5); 	//
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);         	//
	//p5.0 BL A相下降沿
	if(status & GPIO_PIN0) 										
	{
		if(P5IN&(1<<6))//
      EncoderCntBL++;//
		else
			EncoderCntBL--;//
	}
	//p5.6 BL B相下降沿
	else if(status & GPIO_PIN6)
	{
		if(P5IN&(1<<0))
      EncoderCntBL--;
		else
			EncoderCntBL++;
	}
	//p5.4 BR A相下降沿
	else if(status & GPIO_PIN4)
	{
		if(P3IN&(1<<5))//
      EncoderCntBR++;//
		else
			EncoderCntBR--;//
	}
	//p5.1 BL A相上升沿
	else if(status & GPIO_PIN1)
	{
		if(P5IN&(1<<7))
      EncoderCntBL--;
		else
			EncoderCntBL++;//
	}
	//p5.7 BL B相上升沿
	else if(status & GPIO_PIN7)
	{
		if(P5IN&(1<<1))
      EncoderCntBL--;//
		else
			EncoderCntBL++;//
	}
	//p5.5 BR A相上升沿
	else if(status & GPIO_PIN5)
	{
		if(P3IN&(1<<7))//
    	EncoderCntBR++;//
		else
			EncoderCntBR--;//
	}
}
