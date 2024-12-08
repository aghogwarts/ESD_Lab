#include <LPC17xx.h>

unsigned long int i, j, k, x, p;
unsigned char flag, flag1;

void PWM1_IRQHandler(void)
{
	LPC_PWM1->IR = 0xFF; // clear interrupts

	if (flag == 0x00)
	{
		LPC_PWM1->MR4 += 100;
		LPC_PWM1->LER = 0x000000FF;

		if (LPC_PWM1->MR4 >= 2700)
		{
			flag1 = 0xFF;
			flag = 0xFF;

			LPC_PWM1->LER = 0x000000FF;
		}
	}

	else if (flag1 == 0xFF)
	{
		LPC_PWM1->MR4 -= 100;
		LPC_PWM1->LER = 0x000000FF;

		if (LPC_PWM1->MR4 <= 0x500)
		{
			flag = 0x00;
			flag1 = 0x00;
			LPC_PWM1->LER = 0x000000FF;
		}
	}
}

void pwm_init(void)
{
	LPC_SC->PCONP |= (1 << 6);			  // PWM1 is powered
	LPC_PINCON->PINSEL3 &= ~(0x0000C000); // cleared if any other functions are enabled
	LPC_PINCON->PINSEL3 |= 0x00008000;	  // pwm 1.4 is selected for the pin P1.23

	// LPC_PWM1 -> PR = 0x00000000; //Count frequency : Fpclk
	LPC_PWM1->PCR = 0x00001000; // select PWM1 single edge
	LPC_PWM1->MCR = 0x00000002; // Reset and interrupt on PWMMR0
	LPC_PWM1->MR0 = 30000;		// setup match register 0 count
	LPC_PWM1->MR4 = 0x00000100; // setup match register MR1
	LPC_PWM1->LER = 0x000000FF; // enable shadow copy register
	LPC_PWM1->TCR = 0x00000002; // RESET counter and PRESCALER
	LPC_PWM1->TCR = 0x00000009; // enable PWM and counter

	NVIC_EnableIRQ(PWM1_IRQn);
	return;
}

// Assume P0.15 to P0.18 as rows
// Assume P0.19 to P0.22 as columns
int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	pwm_init();

	LPC_PINCON->PINSEL0 = 0;
	LPC_GPIO0->FIODIR = 0x0F << 15; // rows in output and column in input

	while (1)
	{
		LPC_GPIO0->FIOMASK = 0x00780000;
		LPC_GPIO0->FIOPIN = 1 << 15;
		for (k = 0; k < 50; k++)
			;
		LPC_GPIO0->FIOMASK = 0x00000000;

		x = LPC_GPIO0->FIOPIN;
		x = (x >> 19) & 0xF;

		if (x != 0)
		{
			if (x == 1)
			{
				p = 3000;
			}

			else if (x == 2)
			{
				p = 7500;
			}

			else if (x == 4)
			{
				p = 15000;
			}

			else if (x == 8)
			{
				p = 22500;
			}
			LPC_PWM1->MR4 = p;
			LPC_PWM1->LER = 0xFF;
		}

		else
		{
			LPC_PWM1->MR4 = 0;
			LPC_PWM1->LER = 0xFF;
		}
		for (i = 0; i < 1000; i++)
			; // delay
	}
}