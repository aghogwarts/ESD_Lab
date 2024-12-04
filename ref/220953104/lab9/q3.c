#include<LPC17xx.h>

int temp1, temp2, flag1, flag2;

void port_write()
{
	int i;
	LPC_GPIO0 -> FIOPIN = temp2 << 23;
	if(flag1 == 0)
	{
		LPC_GPIO0 -> FIOCLR = 1 << 27;
	}
	
	else
	{
		LPC_GPIO0 -> FIOSET = 1 << 27;
	}
	
	LPC_GPIO0 -> FIOSET = 1 << 28;
	for(i = 0; i < 50; i++);
	LPC_GPIO0 -> FIOCLR = 1 << 28;
	for(i = 0; i < 30000; i++);
}

void lcd_write()
{
	if((flag1 == 0) & ((temp1 == 0x30) || (temp1 == 0x20)))
		flag2 = 1;
	else
		flag2 = 0;
	
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
	int i, k = 0;
	int r,c, n, keycode, total, check = 0, op1 = 0, op2 = 0, exit = 0;
	char s[100], od;
	
	int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};
	
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON -> PINSEL0 = 0;
	LPC_PINCON -> PINSEL3 = 0;
	LPC_PINCON -> PINSEL4 = 0;

	LPC_GPIO0 -> FIODIR = 0xF << 23 | 1 << 27 | 1 << 28;
	LPC_GPIO1 -> FIODIR = 0;
	LPC_GPIO2 -> FIODIR = 0XF << 10;

	while(exit == 0)
	{
		for(r = 0; r < 4; r++)
		{
			LPC_GPIO2 -> FIOPIN = 1 << (r + 10);
			for(i = 0; i < 10000; i++);
			x = LPC_GPIO1 -> FIOPIN;
			x = x >> 23 & 0xF;
			
			if(x != 0)
			{
				if(x == 1)
					c = 0;
				else if(x == 2)
					c = 1;
				else if(x == 4)
					c = 2;
				else if(x == 8)
					c = 3;
				
				keycode = 4 * r + c;
				
				if(keycode < 10)
				{
					s[k++] = keycode;
					if(check == 0)
						op1 = (op1 * 10) + keycode;
					if(check == 1)
						op2 = (op2 * 10) + keycode;
				}
				
				else if(keycode == 10)
				{
					s[k++] = '+';
					od = '+';
					check = 1;
				}
				
				else if(keycode == 11)
				{
					s[k++] = '-';
					od = '-';
					check = 1;
				}
				
				else if(keycode == 12)
				{
					s[k++] = '*';
					od = '*';
					check = 1;
				}
				
				else if(keycode == 13)
				{
					s[k++] = '/';
					od = '/';
					check = 1;
				}
				
				else if(keycode == 14)
				{
					if(od == '+')
						total = op1 + op2;
					else if(od == '-')
						total = op1 - op2;
					else if(od == '*')
						total = op1 * op2;
					else if(od == '/')
						total = op1 / op2;
					
					s[k++] = total;
					exit = 1;
				}
			}
		}
	}
	
	flag1 = 0;
	
	for(i = 0; i <= 8; i++)
	{
		temp1 = lcd_init[i];
		lcd_write();
	}
	flag1 = 1;
	
	for(i = 0; s[i] != '\0'; i++)
	{
		if(i == 16)
		{
			flag1 = 0;
			temp1 = 0xC0;
			lcd_write();
			flag1 = 1;
		}
		
		temp1 = (int) s[i];
		lcd_write();
	}
	
	while(1);
}