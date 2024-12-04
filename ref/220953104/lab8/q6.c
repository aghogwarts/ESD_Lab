#include<LPC17xx.h>

main()
{
	int i, j, k, n = 1, count = 0, temp, x;
	int seven_seg[] = {0x3F, 0x06);
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;	
	LPC_PINCON -> PINSEL1 = 0;
	LPC_PINCON -> PINSEL3 = 0;
	
	LPC_GPIO0 -> FIODIR = 0xFF << 4;
	LPC_GPIO1 -> FIODIR = 0xF << 23;
		
	while(1)
	{
		temp = n;
		x = LPC_GPIO0 -> FIOPIN;
		x = (x >> 21) & 0x1;
		
		if(count == 30000)
		{
			if(x == 0)
			{
				n = n * 10;
				if(n > 1000)
				{
					n = 1;
				}
			}
			
			else
			{
				n = n / 10;
				if(n < 1)
				{
					n = 1000;
				}
			}
			
			count = 0;
		}
		
		for(i = 0; i < 4; i++)
		{
			LPC_GPIO1 -> FIOPIN = i << 23;
			LPC_GPIO0 -> FIOPIN = seven_seg[temp % 10] << 4;
			for(j = 0; j < 100; j++);
			count++;
		}
	}
}