#include <LPC17xx.h> 
#include <stdio.h>

int temp1, temp2, flag1, flag2;

// Function Prototypes
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(unsigned char data);
unsigned char I2C_Read(unsigned char ack);
void ADC_Init(void);
unsigned short ADC_Read(void);
void MAX30102_Init(void);
void MAX30102_WriteRegister(unsigned char reg, unsigned char data);
unsigned char MAX30102_ReadRegister(unsigned char reg);
char* Process_Sensor_Data(void);

//Assume P0.27 for RS
//P0.28 for Enable
//P0.26 to P0.23 for input lines

int temp1, temp2, flag1, flag2;

void port_write()
{
	int i;
	LPC_GPIO0->FIOPIN = temp2 << 23;
	
	if(flag1 == 0)
		LPC_GPIO0->FIOCLR = 1 << 27;
	else
		LPC_GPIO0->FIOSET = 1 << 27;
	
	LPC_GPIO0->FIOSET = 1 << 28;
	for(i = 0; i<50; i++);
	LPC_GPIO0->FIOCLR = 1 << 28;
	
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

/*According to I2C protocols, the data line can not change when the clock line is high, it can change only when the clock line is low. */

/*The data is transmitted in the form of packets which comprises 9 bits. The sequence of these bits are –
Start Condition: 1 bit
Slave Address: 8 bit
Acknowledge: 1 bit
*/

#define MAX30102_ADDRESS 0x57  // I2C address of MAX30102 (7-bit address)
#define ADC_CHANNEL 1          // ADC channel for LM35 (assuming it is connected to P0.24)

int main(void) 
{
    int i, k = 0;
		char *s, od;
	
		int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};
		
		SystemInit();
		SystemCoreClockUpdate();
    
    I2C_Init();        // Initialize I2C for MAX30102
    //ADC_Init();        // Initialize ADC for LM35
    MAX30102_Init();   // Initialize MAX30102

    //for(i = 0; i < 1000; i++);

		while(1)
		{
			s = Process_Sensor_Data();  // Read and print data from both sensors
			
			LPC_PINCON -> PINSEL1 = 0;
	
			LPC_GPIO0 -> FIODIR = 0xF << 23 | 1 << 27 | 1 << 28;
			
			
			flag1 = 0;
			for(i = 0; i<=8; i++)
			{
				temp1 = lcd_init[i];
				LCD_write();
			}
			flag1 = 1;
			//temp1=0x41;
			//LCD_write();
			for(i = 0; s[i]!='\0'; i++)
			{
				if(i == 16)
				{
					flag1 = 0;
					temp1 = 0xC0;
					LCD_write();
					flag1 = 1;
				}
				temp1 = (int) s[i];
				LCD_write();
			}
			for(i = 0; i < 100000; i++);
		}
}

// I2C Initialization for MAX30102
void I2C_Init(void) 
{
    LPC_SC->PCONP |= (1 << 7);   // Power up I2C1
    LPC_PINCON->PINSEL1 |= (1 << 6) | (1 << 8);  // Set P0.19 as SDA, P0.20 as SCL
    LPC_I2C1->I2SCLH = 0x80;     // Set clock high time (adjust for your clock)
    LPC_I2C1->I2SCLL = 0x80;     // Set clock low time (adjust for your clock)
    LPC_I2C1->I2CONSET = 0x40;   // Enable I2C interface
}

/*Start Condition: The master device sends a start condition by pulling the SDA line low while the SCL line is high. This signals that a transmission is about to begin.*/

void I2C_Start(void) 
{
    LPC_I2C1->I2CONSET = 0x20;   // Send START condition
    while (!(LPC_I2C1->I2CONSET & 0x08));  // Wait for START to be transmitted
}

/*Stop Condition: When the transmission is complete, the master sends a stop condition by releasing the SDA line to high while the SCL line is high. This signals that the communication session has ended.*/
void I2C_Stop(void) 
{
    LPC_I2C1->I2CONSET = 0x10;   // Send STOP condition
    LPC_I2C1->I2CONCLR = 0x08;   // Clear interrupt flag
}

/*Data Transmission: The master or slave (depending on the read/write operation) sends data in 8-bit chunks. After each byte, an ACK is sent to confirm that the data has been received successfully.*/
void I2C_Write(unsigned char data) 
{
    LPC_I2C1->I2DAT = data;      // Load data into I2C data register
    LPC_I2C1->I2CONCLR = 0x08;   // Clear SI flag to start transmission
    while (!(LPC_I2C1->I2CONSET & 0x08));  // Wait for transmission to complete
}

unsigned char I2C_Read(unsigned char ack) 
{
    if (ack) 
    {
        LPC_I2C1->I2CONSET = 0x04;  // Send ACK
    } else 
    {
        LPC_I2C1->I2CONCLR = 0x04;  // Send NACK
    }
    LPC_I2C1->I2CONCLR = 0x08;      // Clear SI flag
    while (!(LPC_I2C1->I2CONSET & 0x08));  // Wait for data to be received
    return LPC_I2C1->I2DAT;         // Return received data
}

void MAX30102_Init(void) 
{
    MAX30102_WriteRegister(0x09, 0x03);  // Set to SpO2 mode
    MAX30102_WriteRegister(0x0A, 0x1F);  // Set LED Pulse Amplitude for IR
    MAX30102_WriteRegister(0x0C, 0x1F);  // Set LED Pulse Amplitude for Red
    // Set other configuration parameters as required.
}

void MAX30102_WriteRegister(unsigned char reg, unsigned char data) 
{
    I2C_Start();
    I2C_Write(MAX30102_ADDRESS << 1);  // Send address + write bit
    I2C_Write(reg);                   // Send register address
    I2C_Write(data);                  // Send data
    I2C_Stop();
}

unsigned char MAX30102_ReadRegister(unsigned char reg) 
{
    unsigned char data;
    I2C_Start();
    I2C_Write(MAX30102_ADDRESS << 1);  // Send address + write bit
    I2C_Write(reg);                   // Send register address
    I2C_Start();                      // Send repeated start
    I2C_Write((MAX30102_ADDRESS << 1) | 1); // Send address + read bit
    data = I2C_Read(0);               // Read data (send NACK)
    I2C_Stop();
    return data;
}

// Read data from FIFO for SpO2 and Heart Rate
void MAX30102_ReadFIFO(unsigned int *red_led, unsigned int *ir_led) 
{
    *red_led = MAX30102_ReadRegister(0x05) << 16 | MAX30102_ReadRegister(0x06) << 8 | MAX30102_ReadRegister(0x07);
    *ir_led = MAX30102_ReadRegister(0x08) << 16 | MAX30102_ReadRegister(0x09) << 8 | MAX30102_ReadRegister(0x0A);
}

void Calculate_SpO2_And_HeartRate(unsigned int red_led, unsigned int ir_led, float *heart_rate, float *SpO2) 
{
    // Placeholder for calculation algorithms
    // You need to implement proper algorithms for accurate results
    *heart_rate = (float)(ir_led % 100);  // Dummy processing
    *SpO2 = (float)(red_led % 100);       // Dummy processing
}

// ADC Initialization for LM35
/*void ADC_Init(void) {
    LPC_SC->PCONP |= (1 << 12);   // Power up ADC
    LPC_PINCON->PINSEL1 |= (1 << 14);  // Set P0.24 as AD0.1
    LPC_ADC->ADCR = (1 << 1) |    // Select channel 1 (AD0.1)
                    (4 << 8) |    // ADC clock
                    (1 << 21);    // Enable ADC
}

unsigned short ADC_Read(void) {
    LPC_ADC->ADCR |= (1 << 24);   // Start conversion
    while (!(LPC_ADC->ADGDR & (1 << 31)));  // Wait for conversion to finish
    return (LPC_ADC->ADGDR >> 4) & 0xFFF;   // Return result
}*/

char* Process_Sensor_Data(void) 
{

    // Static array to ensure it retains the data after function exits
    static char stringsa[100];
    // Read and process LM35 data
    /*unsigned short adc_value = ADC_Read();
    float temperature = (adc_value * 3.3 / 4096) * 100; // Assuming 3.3V reference
    */
    // Read example register from MAX30102 (e.g., part ID)
    //unsigned char part_id = MAX30102_ReadRegister(0xFF);  // Replace with actual data you want to read

    unsigned int red_led, ir_led;
    float heart_rate, SpO2;

    // Read data from MAX30102
    MAX30102_ReadFIFO(&red_led, &ir_led);

    // Calculate heart rate and SpO2
    Calculate_SpO2_And_HeartRate(red_led, ir_led, &heart_rate, &SpO2);

    // Print the readings
    //printf("Heart Rate: %.2f BPM\nSpO2: %.2f %%\n", heart_rate, SpO2);
    sprintf(stringsa, "Heart Rate: %.2f BPM\nSpO2: %.2f %%\n", heart_rate, SpO2);

    return stringsa;
    //printf("SpO2: %.2f %%\n", SpO2);
    
    // Print the data
    //printf("LM35 Temperature: %.2f°C\n", temperature);

    //printf("MAX30102 Part ID: %d\n", part_id);
}