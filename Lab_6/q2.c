// Program to turn on LED while switch connected to P2.12 is pressed and OFF when released

#include <LPC17xx.h>

int main(void)
{
	long int x;
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;		// P0.4
	LPC_PINCON -> PINSEL4 = 0;		// P2.12
	
	LPC_GPIO0 -> FIODIR = 1 << 4;	// o/p
	LPC_GPIO2 -> FIODIR = 0;		// i/p
	
	while (1)
	{
		x = LPC_GPIO2 -> FIOPIN;	// Read value at port connected to the switch
		x = x & 1 << 12;			// Bitwise and 
		if (x == 0)
			LPC_GPIO0 -> FIOSET = 1 << 4;
		else
			LPC_GPIO0 -> FIOCLR = 1 << 4;
	}
}