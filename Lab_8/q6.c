// Ring counter

#include <LPC17xx.h>

int main(void)
{
	int seven_seg[] = {0x3F, 0x06};
	int i, j, count = 0, n = 0, temp;

	SystemInit();
	SystemCoreClockUpdate();

	LPC_PINCON->PINSEL0 = 0; // GPIO mode
	LPC_PINCON->PINSEL3 = 0; // GPIO mode

	LPC_GPIO0->FIODIR = 0xFF << 4; // output direction P0.4 to P0.11
	LPC_GPIO1->FIODIR = 0xF << 23; // output direction P1.23 to P1.26

	while (1)
	{
		// Ring counter increment logic (4-bit binary counter)
		if (count == 1000)
		{
			n = (n + 1) % 16; // Count from 0 to 15 (4-bit binary values)
			count = 0;
		}

		// Display the binary counter value on the 7-segment displays
		temp = n;
		for (i = 0; i < 4; i++)
		{
			// Activate the current 7-segment display digit (multiplexing)
			LPC_GPIO1->FIOPIN = (1 << (i + 23)); // Set one of the 4 multiplexing pins high

			// Display the current digit (in binary) on the 7-segment display
			LPC_GPIO0->FIOPIN = seven_seg[temp & 0x01] << 4; // Display least significant bit (LSB)

			temp = temp >> 1; // Shift the number right by 1 (binary shift)

			// Delay to allow display to be visible (multiplexing delay)
			for (j = 0; j < 10000; j++)
				;
		}

		count++; // Increment the counter for the next iteration
	}
}
