// 8-bit binary up/down counter with switch

#include <LPC17xx.h>

int main(void)
{
	long int x;
	int i, j;
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;		// switch
	LPC_PINCON -> PINSEL1 = 0;		// led	
	LPC_GPIO0 -> FIODIR = 0xFF << 15 | 0 << 10;	// P0.15 to P0.22 output direction
	
	i = 0;
	
	while(1)
	{
		x = LPC_GPIO0 -> FIOPIN;
		x = x & 1 << 10;
		if(x == 0) // switch pressed Down counter
		{
			i = i - 1;
			if(i == -1)
				i = 255;
		}
		
		else // UP counter
		{
			i = i + 1;
			if(i == 256)
				i = 0;
		}
		
		LPC_GPIO0 -> FIOPIN = i << 15;
		for(j = 0; j < 1000000; j++);  // delay
	}
}
