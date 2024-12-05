#include <LPC17xx.h>
#include <stdio.h>

//Assume P0.19 for RS
//P0.20 for Enable
//P0.18 to P0.15 for input lines


//Assume P0.27 for RS
//P0.28 for Enable
//P0.26 to P0.23 for input lines


int temp1, temp2, flag1, flag2;

void port_write()
{
	int i;
	LPC_GPIO0->FIOPIN = temp2 << 15;
	
	if(flag1 == 0)
		LPC_GPIO0->FIOCLR = 1 << 19;
	else
		LPC_GPIO0->FIOSET = 1 << 19;
	
	LPC_GPIO0->FIOSET = 1 << 20;
	for(i = 0; i<50; i++);
	LPC_GPIO0->FIOCLR = 1 << 20;
	
	for(i = 0; i<30000; i++);
}

void LCD_write()
{
	if((flag1 == 0) & ((temp1 == 0x30) || (temp1 == 0x20)))
		flag2 = 1;
	else
		flag2 = 0;
	//flag2 = (flag1 == 1) ? 0 :((temp1 == 0x30) || (temp1 == 0x20)) ? 1 : 0;
	temp2 = temp1 >> 4;
	port_write();
	
	if(flag2 == 0)
	{
		temp2 = temp1 & 0xF;
		port_write();
	}
}

int main()
{
	char cm = ' ';
	int temp;
	int adc_temp1;
	int adc_temp2;
	double in_vtg;
	double ref_vtg = 5;
	char dval[50], vtg[50];
	int i;
	//char s[] = "Hot MANAS has got good style, Today is a good day";
	char l1[] = "Temp: ";
	char l2[] = "Soil: ";
	int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};
	SystemInit();
	SystemCoreClockUpdate();
	

	LPC_SC->PCONP |= 1<<15; //Power for the GPIO block
	LPC_SC->PCONP |= 1<<12; //enable the peripheral ADC

	LPC_PINCON -> PINSEL1 = 0 | 1<<18 | 1<<14;
	//LPC_PINCON -> PINSEL3 = 3<<30; //P1.31 in func 3 for AD0.5
	
	LPC_GPIO0 -> FIODIR = 0xF << 15 | 1 << 19 | 1 << 20;
	LPC_GPIO1 -> FIODIR = 1<<23;

	LPC_GPIO1 -> FIOSET = 1<<23;
	
	
	flag1 = 0;
	for(i = 0; i<=8; i++)
	{
		temp1 = lcd_init[i];
		LCD_write();
	}
	
	//line 1 writing
	flag1 = 0;
	temp1 = 0x80;
	LCD_write();
	flag1 = 1;
	for(i = 0; l1[i]!='\0'; i++)
	{
		temp1 = l1[i];
		LCD_write();
	}
	
	//line 2 writing
	flag1 = 0;
	temp1 = 0xC0;
	LCD_write();
	flag1 = 1;
	for(i = 0; l2[i]!='\0'; i++)
	{
		temp1 = l2[i];
		LCD_write();
	}
	
	while(1)
	{
		LPC_ADC -> ADCR =  1<< 2| 1<<0 | 1<<16 | 1<< 21 ;
		
		while(!((LPC_ADC -> ADDR0)>>31 & 1));
		
		adc_temp2 = LPC_ADC -> ADDR0;
		adc_temp2 = (adc_temp2 >> 4) & (0xFFF);
		
		in_vtg = (((float) adc_temp2 * (float) 3.3)/((float) 4096.0));
		in_vtg =(int)(in_vtg * 100);
		temp=(int)in_vtg;
		sprintf(vtg, "%d", temp);
		

		//LPC_ADC -> ADCR = 1<<2 | 1<< 21 | 1 <<24 ;
		
		while(!((LPC_ADC -> ADDR2)>>31 & 1));
		
		adc_temp1 = LPC_ADC -> ADDR2;
		adc_temp1 = (adc_temp1 >> 4) & (0xFFF);
		
		if(adc_temp1 > 4000)
			cm = 'A';
		else if(adc_temp1 > 2400)
			cm = 'D';
		else if(adc_temp1 >1480)
			cm = 'H';
		else 
			cm = 'W';
		sprintf(dval, "%c", cm);
	
		
		
		
		flag1 = 0;
		temp1 = 0x86;
		LCD_write();
		flag1 = 1;
		for(i = 0; vtg[i]!='\0'; i++)
		{
			temp1 = vtg[i];
			LCD_write();
		}
		
		flag1 = 0;
		temp1 = 0xC6;
		LCD_write();
		flag1 = 1;
		for(i = 0; dval[i]!='\0'; i++)
		{
			temp1 = dval[i];
			LCD_write();
		}
	}
}