#include <LPC17xx.h>
#include <stdlib.h>

// die tossing
// Assume switch configured to P2.12
// Assume P0.27 for RS
// Assume P0.28 to Enable
// Assume P0.26 to P0.23 for input lines

int temp1, temp2, flag1, flag2;

void port_write()
{
	int i;
	LPC_GPIO0->FIOPIN = temp2 << 23;
	if (flag1 == 0) // command mode
	{
		LPC_GPIO0->FIOCLR = 1 << 27;
	}

	else // data mode
	{
		LPC_GPIO0->FIOSET = 1 << 27;
	}

	LPC_GPIO0->FIOSET = 1 << 28;
	for (i = 0; i < 50; i++)
		;
	LPC_GPIO0->FIOCLR = 1 << 28;
	for (i = 0; i < 30000; i++)
		; // delay for LCD
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

int main(void)
{
	long int x;
	int i;

	char num;
	int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};

	SystemInit();
	SystemCoreClockUpdate();

	LPC_PINCON->PINSEL0 = 0;
	LPC_PINCON->PINSEL4 = 0; // P2.12

	LPC_GPIO0->FIODIR = 0xF << 23 | 1 << 27 | 1 << 28;
	LPC_GPIO2->FIODIR = 0; // input

	while (1)
	{
		x = LPC_GPIO2->FIOPIN;
		x = x & 1 << 12;
		if (x == 0)
		{
			num = rand() % 6 + 1; // use rand() to generate a random number between 1 and 6
			flag1 = 0;
			for (i = 0; i <= 8; i++)
			{
				temp1 = lcd_init[i];
				lcd_write();
			}
			flag1 = 1;

			temp1 = (int)num;
			lcd_write();
			for (i = 0; i < 100000; i++)
				; // delay
		}
	}
}