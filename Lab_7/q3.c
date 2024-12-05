// Ring counter with switch

#include <LPC17xx.h>

int main(void)
{
	long int x;
	int i, j;
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;		// GPIO mode P0.4 to P0.11
	LPC_PINCON -> PINSEL4 = 0;		// P2.12
	
	LPC_GPIO0 -> FIODIR = 0xFF << 4;	// P0.4 to P0.11 output direction
	LPC_GPIO2 -> FIODIR = 0;			// input P2.12
	
	i = 0;
	
	while(1)
	{
		x = LPC_GPIO2 -> FIOPIN;
		x = x & 1 << 12;
		
		if(x == 0) // switch pressed
		{
			// see clr code
			if(i == 0)
				LPC_GPIO0 -> FIOCLR = 1 << 11;
			else
				LPC_GPIO0 -> FIOCLR = 1 << (i + 3);
			
			LPC_GPIO0 -> FIOSET = 1 << (i + 4);
			i += 1;
			if(i == 8)
				i = 0;
		}
		
		for(j = 0; j < 1000000; j++);  // delay
	}
}
		