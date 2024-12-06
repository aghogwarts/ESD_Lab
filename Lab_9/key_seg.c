#include <LPC17xx.h>

main()
{
	int seven_seg[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	int i, j, k, temp, col, count = 0, keyboad;
	long int x;
	SystemInit();
	SystemCoreClockUpdate();

	LPC_PINCON->PINSEL0 = 0; // GPIO mode
	LPC_PINCON->PINSEL1 = 0; // GPIO mode
	LPC_PINCON->PINSEL3 = 0; // GPIO mode
	LPC_PINCON->PINSEL4 = 0; // GPIO mode

	LPC_GPIO0->FIODIR = 0x78FF0;   // to set the output direction
	LPC_GPIO1->FIODIR = 0;		   // row set to input direction
	LPC_GPIO2->FIODIR = 0xF << 10; // column set to output direction

	LPC_GPIO0->FIOPIN = 0; // to enable LSB segment

	while (1)
	{
		for (j = 0; j < 4; j++)
		{
			LPC_GPIO2->FIOPIN = 1 << j + 10;
			for (k = 0; k < 50; k++)
				;
			x = LPC_GPIO1->FIOPIN;
			x = (x >> 23) & 0xF;

			if (x != 0)
			{
				if (x == 1)
					col = 0;
				else if (x == 2)
					col = 1;
				else if (x == 4)
					col = 2;
				else if (x == 8)
					col = 3;

				keyboad = (4 * j) + col;

				LPC_GPIO0->FIOMASK = 0xFFFFF00F;
				LPC_GPIO0->FIOPIN = seven_seg[keyboad] << 4;

				for (j = 0; j < 10000; j++)
					;
			}
		}
	}
}