#include<LPC17xx.h>

//switch p2.12
//LED p0.4
main()
{
	long int x;
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0; //P0.4
	LPC_PINCON -> PINSEL4 = 0; //P2.12
	
	LPC_GPIO0 -> FIODIR = 1 << 4; //output
	LPC_GPIO2 -> FIODIR = 0; //input
	
	while(1)
	{
		x = LPC_GPIO2 -> FIOPIN;
		x = x & 1 << 12;
		if(x == 1)
			LPC_GPIO0 -> FIOSET = 1 << 4;
		else
			LPC_GPIO0 -> FIOCLR = 1 << 4;
	}
}