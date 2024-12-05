// 8 bit binary up counter

#include <LPC17xx.h>

// CNA P0.4 to P0.11

int main(void)
{
	int i, j;
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;			// GPIO mode
	LPC_GPIO0 -> FIODIR = 0xFF << 4; 	// output direction
	
	while(1)
	{
		for(i = 0; i < 256; i++)
		{
			LPC_GPIO0 -> FIOPIN = i << 4;   
			for(j = 0; j < 1000000; j++);  // delay
		}
	}
}
	