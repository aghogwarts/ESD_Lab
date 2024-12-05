// Program to blink all LEDs connected from P0.4 to P0.11

#include <LPC17xx.h>

int main(void)
{
	int i;
	
	SystemInit();
	SystemCoreClockUpdate();
		
	LPC_PINCON -> PINSEL0 = 0;		// Set fn mode GPIO
	LPC_GPIO0 -> FIODIR = 0xFF << 4;	// Set direction to out 
	
	while (1)
	{
		LPC_GPIO0 -> FIOSET = 0xFF << 4;	//set logic 1 to LED
		for (i = 0; i < 1000000; i++);
		LPC_GPIO0 -> FIOCLR = 0xFF << 4;	//set logic 0 to LED
		for (i = 0; i < 1000000; i++);
	}
}