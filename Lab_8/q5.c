#include<LPC17xx.h>
//UP / Down counter
main()
{
	int seven_seg[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	int i, j, temp, n;
	long int x;
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0; //GPIO mode
	LPC_PINCON -> PINSEL3 = 0; //GPIO mode
	LPC_PINCON -> PINSEL4 = 0;// P2.12

	LPC_GPIO0 -> FIODIR = 0xFF << 4; //output direction P0.4 to P0.11
	LPC_GPIO1 -> FIODIR = 0xF << 23; //output direction P1.23 to P1.26
	LPC_GPIO2 -> FIODIR = 0; //input P2.12

	n = 0;
	
	while(1)
	{
		x = LPC_GPIO2 -> FIOPIN;
		x = x & 1 << 12;
		
		if(x == 0)  //switch pressed down counter
		{
			n = n - 1;
			if(n == -1)
			{
				n = 0xFFFF;
			}
		}
		
		else //UP counter
		{
			n = n + 1;
			if(n == 0x10000)
			{
				n = 0;
			}
		}
		
		temp = n;
		//Display
		for(i = 0; i < 4; i++)
		{
			LPC_GPIO1 -> FIOPIN = i << 23;
			LPC_GPIO0 -> FIOPIN = seven_seg[temp % 0x10] << 4;
			temp = temp / 0x10;
			for(j = 0; j < 1000000; j++);
		}
	}
}