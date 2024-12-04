#include<LPC17xx.h>
//turn on led p0.4
main()
{
	int i;
	SystemInit();
	SystemCoreClockUpdate();
	
	//set function mode
	
	LPC_PINCON -> PINSEL0 = 0; //GPIO MODE
	LPC_GPIO0 -> FIODIR = 1 << 4; //Set directiom
	
	while(1)
	{
		LPC_GPIO0 -> FIOSET = 1 << 4; //set logic 1 to LED
		for(i = 0; i < 1000000; i++);
		LPC_GPIO0 -> FIOCLR = 1 << 4; //set logic 0 to LED
		for(i = 0; i < 1000000; i++);
	}
}