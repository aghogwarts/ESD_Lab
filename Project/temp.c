#include <LPC17xx.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
// Assume P0.19 for RS
// P0.20 for Enable
// P0.18 to P0.15 for input lines

int temp1, temp2, flag1, flag2;

void port_write()
{
	int i;
	LPC_GPIO0->FIOPIN = temp2 << 15;

	if (flag1 == 0)
		LPC_GPIO0->FIOCLR = 1 << 19;
	else
		LPC_GPIO0->FIOSET = 1 << 19;

	LPC_GPIO0->FIOSET = 1 << 20;
	for (i = 0; i < 50; i++)
		;
	LPC_GPIO0->FIOCLR = 1 << 20;

	for (i = 0; i < 30000; i++)
		;
}

void LCD_write()
{
	if ((flag1 == 0) & ((temp1 == 0x30) || (temp1 == 0x20)))
		flag2 = 1;
	else
		flag2 = 0;
	// flag2 = (flag1 == 1) ? 0 :((temp1 == 0x30) || (temp1 == 0x20)) ? 1 : 0;
	temp2 = temp1 >> 4;
	port_write();

	if (flag2 == 0)
	{
		temp2 = temp1 & 0xF;
		port_write();
	}
}

int main()
{
	int temp;
	int adc_temp;
	double in_vtg;
	char vtg[50];
	int i;

	char l1[] = "Body Temp: ";

	int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};
	SystemInit();
	SystemCoreClockUpdate();

	LPC_SC->PCONP |= 1 << 15; // Power for the GPIO block
	LPC_SC->PCONP |= 1 << 12; // enable the peripheral ADC

	LPC_PINCON->PINSEL1 = 0 | 1 << 18; // AD0.2 in P0.25
	// LPC_PINCON -> PINSEL3 = 3<<30; //P1.31 in func 3 for AD0.5

	LPC_GPIO0->FIODIR = 0xF << 15 | 1 << 19 | 1 << 20;
	LPC_GPIO1->FIODIR = 1 << 23;

	LPC_GPIO1->FIOSET = 1 << 23;

	flag1 = 0;
	for (i = 0; i <= 8; i++)
	{
		temp1 = lcd_init[i];
		LCD_write();
	}

	// line 1 writing
	flag1 = 0;
	temp1 = 0x80;
	LCD_write();
	flag1 = 1;
	for (i = 0; l1[i] != '\0'; i++)
	{
		temp1 = l1[i];
		LCD_write();
	}

	while (1)
	{
		LPC_ADC->ADCR = 1 << 2 | 1 << 21 | 1 << 24;

		while (!((LPC_ADC->ADGDR) >> 31 & 1))
			;

		adc_temp = LPC_ADC->ADGDR;
		adc_temp = (adc_temp >> 4) & (0xFFF);

		in_vtg = (((float)adc_temp * (float)3.3) / ((float)4096.0));
		in_vtg = (int)(in_vtg * 100);
		temp = (int)in_vtg;

		sprintf(vtg, "%d", temp);

		flag1 = 0;
		temp1 = 0x8B;
		LCD_write();
		flag1 = 1;
		for (i = 0; vtg[i] != '\0'; i++)
		{
			temp1 = vtg[i];
			LCD_write();
		}

		for (i = 0; i < 30000; i++)
			; // delay
	}
}
