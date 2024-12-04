#include <LPC17xx.H>

void pwm_init(void);
void PWM1_IRQHandler(void);

unsigned long int i;
unsigned char flag,flag1;

int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	pwm_init();
	while(1)
	{
		for(i=0;i<=1000;i++); // delay
	}//end of while
}//end of main

void pwm_init(void)
{
	LPC_SC->PCONP |= (1<<6); //PWM1 is powered
	LPC_PINCON->PINSEL3 &= ~(0x0000C000); //cleared if any other
	//functions are enabled
	LPC_PINCON->PINSEL3 |= 0x00008000; //pwm1.4 is selected for the pin
	//P1.23
	//LPC_PWM1->PR = 0x00000000; //Count frequency : Fpclk
	LPC_PWM1->PCR = 0x00001000; //select PWM1 single edge
	LPC_PWM1->MCR = 0x00000003; //Reset and interrupt on PWMMR0
	LPC_PWM1->MR0 = 30000; //setup match register 0 count
	LPC_PWM1->MR4 = 0x00000100; //setup match register MR1
	LPC_PWM1->LER = 0x000000FF; //enable shadow copy register
	LPC_PWM1->TCR = 0x00000002; //RESET COUNTER AND PRESCALER
	LPC_PWM1->TCR = 0x00000009; //enable PWM and counter
	NVIC_EnableIRQ(PWM1_IRQn);
	return;
}

void PWM1_IRQHandler(void)
{
	LPC_PWM1->IR = 0xFF; //clear the interrupts
	if(flag == 0x00)
	{
		LPC_PWM1 -> MR4 += 100;
		LPC_PWM1 -> LER = 0x000000FF;
		if(LPC_PWM1 -> MR4 >= 27000)
		{
			flag1 = 0xFF;
			flag = 0xFF;
			LPC_PWM1 -> LER = 0x000000FF;
		}
	}
	
	else if(flag1 == 0xff)
	{
		LPC_PWM1 -> MR4 -= 100;
		LPC_PWM1 -> LER = 0x000000fF;
		if(LPC_PWM1 -> MR4 <= 0x500)
		{
			flag = 0x00;
			flag1 = 0x00;
			LPC_PWM1 -> LER = 0X000000FF;
		}
	}
}