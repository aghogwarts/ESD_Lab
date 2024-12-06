#include <LPC17xx.h>

// Assume P0.27 for RS
// P0.28 for Enable
// P0.26 to P0.23 for input lines

int temp1, temp2, flag1, flag2;

void port_write()
{
	int i;
	LPC_GPIO0->FIOPIN = temp2 << 23;

	if (flag1 == 0)
		LPC_GPIO0->FIOCLR = 1 << 27;
	else
		LPC_GPIO0->FIOSET = 1 << 27;

	LPC_GPIO0->FIOSET = 1 << 28;
	for (i = 0; i < 50; i++)
		;
	LPC_GPIO0->FIOCLR = 1 << 28;

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
	int i;
	char s[] = "Hello CCE";
	int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};
	SystemInit();
	SystemCoreClockUpdate();

	LPC_PINCON->PINSEL1 = 0;

	LPC_GPIO0->FIODIR = 0xF << 23 | 1 << 27 | 1 << 28;

	flag1 = 0;
	for (i = 0; i <= 8; i++)
	{
		temp1 = lcd_init[i];
		LCD_write();
	}
	flag1 = 1;
	// temp1=0x41;
	// LCD_write();
	for (i = 0; s[i] != '\0'; i++)
	{
		if (i == 16)
		{
			flag1 = 0;
			temp1 = 0xC0;
			LCD_write();
			flag1 = 1;
		}
		temp1 = (int)s[i];
		LCD_write();
	}
	while (1)
		;
}