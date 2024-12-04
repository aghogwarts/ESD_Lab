#include<LPC17xx.h>
// one digit hex counter
// q2 4 digit static number
// q3 up/down counter for 4 digit
main()
{
	int i = 0;
	int j;
	int seven_seg[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0; //GPIO mode
	LPC_PINCON -> PINSEL3 = 0; //GPIO mode

	LPC_GPIO0 -> FIODIR = 0xFF0; //output direction
	LPC_GPIO1 -> FIODIR = 0xF << 23;

	LPC_GPIO1 -> FIOPIN = 0 << 23;
	while(1)
	{
		for(i = 0; i < 16; i++)
		{
			LPC_GPIO0 -> FIOPIN = (seven_seg[i]) << 4;
			for(j = 0; j < 10000000; j++); //delay
		}
	}
}