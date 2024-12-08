#include <LPC17xx.h>
#include <stdio.h>

#define Ref_Vtg 3.300
#define Full_Scale 0xFFF // 12 bit ADC

// diff between dual channel ADC input and display diff

// Assume P0.27 for RS
// P0.28 for Enable
// P0.26 to P0.23 for input lines

int flag1, flag2, temp1, temp2;

void port_write()
{
	int i;
	LPC_GPIO0->FIOPIN = temp2 << 23;
	if (flag1 == 0)
	{
		LPC_GPIO0->FIOCLR = 1 << 27;
	}

	else
	{
		LPC_GPIO0->FIOSET = 1 << 27;
	}

	LPC_GPIO0->FIOSET = 1 << 28;
	for (i = 0; i < 50; i++)
		;
	LPC_GPIO0->FIOCLR = 1 << 28;
	for (i = 0; i < 30000; i++)
		;
}

void lcd_write()
{
	if ((flag1 == 0) & ((temp1 == 0x30) || (temp1 == 0x20)))
		flag2 = 1;
	else
		flag2 = 0;

	temp2 = temp1 >> 4;
	port_write();
	if (flag2 == 0)
	{
		temp2 = temp1 & 0xF;
		port_write();
	}
}

void lcd_initialize()
{
	int i;
	int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};
	LPC_PINCON->PINSEL1 = 0;
	LPC_GPIO0->FIODIR = 0xF << 23 | 1 << 27 | 1 << 28;

	flag1 = 0; // command mode

	for (i = 0; i <= 8; i++)
	{
		temp1 = lcd_init[i];
		lcd_write();
	}
}

int main(void)
{
	unsigned long adc_temp1, adc_temp2;
	int i;
	float in_vtg1, in_vtg2, diff;
	unsigned char vtg[7], dval[7];
	unsigned char msg3[12] = {"Difference: "};

	SystemInit();
	SystemCoreClockUpdate();

	LPC_SC->PCONP |= (1 << 15); // Power for GPIO block
	lcd_initialize();			// initialize the lcd

	LPC_PINCON->PINSEL3 = 3 << 28 | 3 < 30; // P1.30 as AD0.4 and P1.31 as AD0.5
	LPC_SC->PCONP |= 1 << 12;				// enable the peripheral ADC

	flag1 = 0; // command mode
	temp1 = 0x80;
	lcd_write();

	flag1 = 1; // Data mode
	i = 0;

	while (msg3[i] != '\0')
	{
		temp1 = msg3[i];
		lcd_write();
		i++;
	}

	while (1)
	{
		LPC_ADC->ADCR = (1 << 4) | (1 << 21) | (1 << 24); // ADC 0.5, start converstion and operational
		while ((adc_temp1 = LPC_ADC->ADDR4) == 0x80000000)
			;
		adc_temp1 = LPC_ADC->ADGDR;
		adc_temp1 >>= 4;
		adc_temp1 &= 0x00000FFF;
		in_vtg1 = (((float)adc_temp1 * (float)Ref_Vtg)) / ((float)Full_Scale); // calculating input analog voltage

		LPC_ADC->ADCR = (1 << 5) | (1 << 21) | (1 << 24); // ADC 0.5, start converstion and operational
		while ((adc_temp2 = LPC_ADC->ADDR5) == 0x80000000)
			;
		adc_temp2 = LPC_ADC->ADGDR;
		adc_temp2 >>= 4;
		adc_temp2 &= 0x00000FFF;
		in_vtg2 = (((float)adc_temp2 * (float)Ref_Vtg)) / ((float)Full_Scale); // calculating input analog voltage

		diff = in_vtg2 - in_vtg1;

		sprintf(vtg, "%3.2fV", diff);

		for (i = 0; i < 2000; i++)
			;

		flag1 = 0;
		temp1 = 0x8C;
		lcd_write();
		flag1 = 1;

		i = 0;
		while (vtg[i] != '\0')
		{
			temp1 = vtg[i];
			lcd_write();
			i++;
		}

		for (i = 0; i < 7; i++)
		{
			vtg[i] = dval[i] = 0;
		}
	}
}