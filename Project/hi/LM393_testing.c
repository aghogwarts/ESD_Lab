#include <LPC17xx.h>
#include <stdio.h>

// Assume P0.19 for RS
// P0.20 for Enable
// P0.18 to P0.15 for input lines

int temp1, temp2, flag1, flag2;

void port_write() {
    int i;
    LPC_GPIO0->FIOPIN = temp2 << 15;

    if (flag1 == 0)
        LPC_GPIO0->FIOCLR = 1 << 19;  // RS = 0 for command
    else
        LPC_GPIO0->FIOSET = 1 << 19;  // RS = 1 for data

    LPC_GPIO0->FIOSET = 1 << 20;  // Enable HIGH
    for (i = 0; i < 50; i++);
    LPC_GPIO0->FIOCLR = 1 << 20;  // Enable LOW

    for (i = 0; i < 30000; i++);
}

void LCD_write() {
    if ((flag1 == 0) && ((temp1 == 0x30) || (temp1 == 0x20)))
        flag2 = 1;
    else
        flag2 = 0;

    temp2 = temp1 >> 4;
    port_write();

    if (flag2 == 0) {
        temp2 = temp1 & 0xF;
        port_write();
    }
}

int main() {
    int adc_temp;
    double in_vtg;
    char dval[50], vtg[50];
    int i;
    char l1[] = "LDR Voltage: ";
    char l2[] = "ADC Value: ";
    int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};

    SystemInit();
    SystemCoreClockUpdate();

    LPC_SC->PCONP |= 1 << 15; // Power for the GPIO block
    LPC_SC->PCONP |= 1 << 12; // Enable the peripheral ADC

    LPC_PINCON->PINSEL1 = 0 | 1 << 18; // Set P0.18 as ADC input (AD0.2)

    LPC_GPIO0->FIODIR = 0xF << 15 | 1 << 19 | 1 << 20; // LCD control pins
    LPC_GPIO1->FIODIR = 1 << 23; // Assume P1.23 for LED output

    // Initialize LCD
    flag1 = 0;
    for (i = 0; i <= 8; i++) {
        temp1 = lcd_init[i];
        LCD_write();
    }

    // Line 1 writing
    flag1 = 0;
    temp1 = 0x80;
    LCD_write();
    flag1 = 1;
    for (i = 0; l1[i] != '\0'; i++) {
        temp1 = l1[i];
        LCD_write();
    }

    // Line 2 writing
    flag1 = 0;
    temp1 = 0xC0;
    LCD_write();
    flag1 = 1;
    for (i = 0; l2[i] != '\0'; i++) {
        temp1 = l2[i];
        LCD_write();
    }

    while (1) {
        LPC_ADC->ADCR = 1 << 2 | 1 << 21 | 1 << 24; // Start ADC conversion

        while (!((LPC_ADC->ADGDR) >> 31 & 1)); // Wait for conversion

        adc_temp = LPC_ADC->ADGDR;
        adc_temp = (adc_temp >> 4) & (0xFFF); // Get the ADC value

        in_vtg = (((float)adc_temp * (float)3.3) / ((float)4096.0)); // Convert to voltage

        sprintf(vtg, "%3.2fV", in_vtg);
        sprintf(dval, "%x", adc_temp);

        flag1 = 0;
        temp1 = 0x8B; // Set cursor for LDR voltage
        LCD_write();
        flag1 = 1;
        for (i = 0; vtg[i] != '\0'; i++) {
            temp1 = vtg[i];
            LCD_write();
        }

        flag1 = 0;
        temp1 = 0xC8; // Set cursor for ADC value
        LCD_write();
        flag1 = 1;
        for (i = 0; dval[i] != '\0'; i++) {
            temp1 = dval[i];
            LCD_write();
        }

        // Example output control based on LDR voltage
        if (in_vtg > 1.5) { // Adjust threshold as needed
            LPC_GPIO1->FIOSET = 1 << 23; // Turn on LED
        } else {
            LPC_GPIO1->FIOCLR = 1 << 23; // Turn off LED
        }
    }
}