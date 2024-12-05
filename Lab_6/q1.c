// Program to blink an LED connected at P0.4

#include <LPC17xx.h>

int main(void)
{
	int i;
	
	SystemInit();
	SystemCoreClockUpdate();
		
	LPC_PINCON -> PINSEL0 = 0;		// Set fn mode GPIO
	LPC_GPIO0 -> FIODIR = 1 << 4;	// Set direction to out 
	
	while (1)
	{
		LPC_GPIO0 -> FIOSET = 1 << 4;	//set logic 1 to LED
		for (i = 0; i < 1000000; i++);
		LPC_GPIO0 -> FIOCLR = 1 << 4;	//set logic 0 to LED
		for (i = 0; i < 1000000; i++);
	}
}