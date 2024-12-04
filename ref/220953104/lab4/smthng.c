#include<LPC17xx.h>


unsigned int i=0,j=0 ;
int main()
LPC_PINCON -> PINSEL0&=0xFFFFFFFF;
LPC_GPIO0-> FIODIR|= 0xFF<<4;	
			                                          

while(1){
	LPC_GPIO0->FIOSET=0xFF<<4;
	for(i = 0; i<1000;i++);
	LPC_GPIO0->FIOCLR=0xFF<<4;
  for(j = 0; j<1000;j++);
	
}
	
	
	
}

