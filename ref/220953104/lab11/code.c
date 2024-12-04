#include <LPC17xx.H>
void pwm_init(void);
//void PWM1_IRQHandler(void);
unsigned long int i;
unsigned char flag,flag1;
int main(void)
{
//assume p0.15 to p0.18 for rows in output and
	// p019 to p0.22 for columns in input
	int p = 0;
	int x, row, col = 0, j, arrval;
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;
	LPC_PINCON -> PINSEL1 = 0;
	//LPC_PINCON -> PINSEL3 = 0;
	
	pwm_init();
	
	
	LPC_GPIO0->FIODIR = 0xF << 19; // p0.15 to p0.18 for row in output
	//LPC_GPIO1->FIODIR = 0;
	//LPC_GPIO2->FIODIR = 0xF<<10;

	//LPC_GPIO0 -> FIOPIN = 0;
	
	
	
	LPC_GPIO0 ->FIOPIN = 1 << (19);
	LPC_GPIO0 -> FIOMASK = 1 << 19;
	
	while(1)
	{
		
		for(j = 0; j<50; j++);
		x = LPC_GPIO0->FIOPIN;
		x = x >> 15 & 0xF;
		if(x)
		{
			if(x == 8)
				p = 75;//p = 22500; //col = 3;
			else if(x == 4)
				p = 50;//p = 15000; //col = 2;
			else if(x == 2)
				p = 25;//p = 7500; //col = 1;
			else if(x == 1)
				p = 10; //p = 3000; //col = 0;

			//Display on Seven Seg
			LPC_PWM1 -> MR4 = p*300;
			LPC_PWM1 -> LER = 0xFF;
			//LPC_GPIO0->FIOPIN = seven_seg[arrval]<<4;
			
		}
		else
		{
			LPC_PWM1 -> MR4 = 0;
			LPC_PWM1 -> LER = 0xFF;
		}
	}
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
	LPC_PWM1->MCR = 0x00000002; //Reset and interrupt on PWMMR0;3 og
	LPC_PWM1->MR0 = 30000; //setup match register 0 count
	LPC_PWM1->MR4 = 0x00000100; //setup match register MR1
	LPC_PWM1->LER = 0x000000FF; //enable shadow copy register
	LPC_PWM1->TCR = 0x00000002; //RESET COUNTER AND PRESCALER
	LPC_PWM1->TCR = 0x00000009; //enable PWM and counter
	//NVIC_EnableIRQ(PWM1_IRQn);
	return;
}

/*
void PWM1_IRQHandler(void)
{
LPC_PWM1->IR = 0xff; //clear the interrupts
if(flag == 0x00)
{
LPC_PWM1->MR4 += 100;
LPC_PWM1->LER = 0x000000FF;
if(LPC_PWM1->MR4 >= 27000)
{
flag1 = 0xff;
flag = 0xff;
LPC_PWM1->LER = 0x000000fF;
}
}
else if(flag1 == 0xff)
{
LPC_PWM1->MR4 -= 100;
LPC_PWM1->LER = 0x000000fF;
if(LPC_PWM1->MR4 <= 0x500)
{
flag = 0x00;
flag1 = 0x00;
LPC_PWM1->LER = 0X000000fF;
}
}
}
*/
