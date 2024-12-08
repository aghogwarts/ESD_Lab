#include "LPC17xx.h"
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 100 // Sample rate (100 samples per second)

#define MAX30102_ADDR 0x57 // 7-bit I2C address of MAX30102
#define FILTER_SIZE 5      // Size of the moving average filter

#define FIFO_READY_MASK 0x01 // FIFO data ready mask (bit 0 of FIFO_STATUS register)

#define PEAK_THRESHOLD 1000 // Minimum amplitude required to detect a peak
#define MIN_DISTANCE 40     // Minimum distance between peaks (in terms of samples)

static uint32_t peak_times[10];     // Buffer to store the times of detected peaks
static uint8_t peak_index = 0;      // Index to store the most recent peak time
static uint32_t last_peak_time = 0; // The time of the last peak
static int32_t last_peak_value = 0;

void wait_ms(uint32_t ms)
{
    uint32_t count = ms * 1000; // Convert milliseconds to microseconds
    while (count--)
        ;
}

// Initialize I2C
// I2C0 pins(SDA and SCL) mapped to P0.27(SDA) and P0.28(SCL)

// Initialize I2C communication
void I2C0_Init(void)
{
    LPC_SC->PCONP |= (1 << 7);                    // Enable power to I2C0
    LPC_PINCON->PINSEL1 |= (1 << 22) | (1 << 24); // Set P0.27 and P0.28 for I2C0 SDA/SCL
    LPC_I2C0->I2CONCLR = 0x6C;                    // Clear I2C control register
    LPC_I2C0->I2SCLH = 0x80;                      // Set the clock for I2C (100kHz)
    LPC_I2C0->I2SCLL = 0x80;
    LPC_I2C0->I2CONSET = (1 << 6); // Enable I2C interface
}

// Generate Start condition on I2C bus
void I2C0_Start(void)
{
    LPC_I2C0->I2CONCLR = (1 << 3) | (1 << 5); // Clear any existing flags
    LPC_I2C0->I2CONSET = (1 << 5);            // Generate START condition
}

// Generate Stop condition on I2C bus
void I2C0_Stop(void)
{
    LPC_I2C0->I2CONSET = (1 << 4); // Generate STOP condition
}

// Send a byte of data to the I2C bus
void I2C0_Write(uint8_t data)
{
    LPC_I2C0->I2DAT = data;        // Put data into the data register
    LPC_I2C0->I2CONCLR = (1 << 3); // Clear the interrupt flag
    LPC_I2C0->I2CONSET = (1 << 1); // Enable the transmit interrupt
    while (!(LPC_I2C0->I2CONSET & (1 << 3)))
        ; // Wait until the transfer is done
}

// Reads a byte of data from the I2C bus
uint8_t I2C0_Read(void)
{
    LPC_I2C0->I2CONCLR = (1 << 3); // Clear the interrupt flag
    LPC_I2C0->I2CONSET = (1 << 2); // Enable the receive interrupt
    while (!(LPC_I2C0->I2CONSET & (1 << 3)))
        ;                   // Wait until the data is received
    return LPC_I2C0->I2DAT; // Return received data
}

// Configure MAX30102

/*
2.1 MAX30102 Register Addresses
Here are some key registers of the MAX30102 you will need to interact with (consult the datasheet for more details):

0x06: Mode Configuration (set mode of operation)
0x07: SpO2 Configuration (set sample rate, LED drive, etc.)
0x09: LED Configuration (configure LED current)
0x01: Reset register
*/

// below code is using the above registers so please check them carefully @ANSH

// Initialize MAX30102
//  Write to a MAX30102 register
void MAX30102_Write(uint8_t reg, uint8_t value)
{
    I2C0_Start();
    I2C0_Write(MAX30102_ADDR << 1); // Send the 8-bit address with the write bit (0)
    I2C0_Write(reg);                // Send the register address
    I2C0_Write(value);              // Write the data to the register
    I2C0_Stop();
}

// Read from a MAX30102 register
uint8_t MAX30102_Read(uint8_t reg)
{
    uint8_t value;
    I2C0_Start();
    I2C0_Write(MAX30102_ADDR << 1);       // Send the 8-bit address with the write bit (0)
    I2C0_Write(reg);                      // Send the register address
    I2C0_Start();                         // Restart condition
    I2C0_Write((MAX30102_ADDR << 1) | 1); // Send address with read bit (1)
    value = I2C0_Read();                  // Read the data
    I2C0_Stop();
    return value;
}

// Initialize MAX30102
void MAX30102_Init(void)
{
    volatile int i;
    // Soft reset
    MAX30102_Write(0x01, 0x40); // Reset register
    for (i = 0; i < 10000; i++)
        ; // Delay for reset completion

    // Set mode: heart rate and SpO2 mode
    MAX30102_Write(0x09, 0x03); // Set Mode Config register for SpO2 mode (Red and IR LEDs)

    // Set SpO2 configuration: LED current = 4x, sample rate = 100 samples per second
    MAX30102_Write(0x0A, 0x21); // Set SpO2 Config register: 100 samples/sec, 16-bit resolution (118  s pulse width)

    // Set LED configuration: Red LED current = 0x0F (maximum)
    MAX30102_Write(0x0C, 0x0F); // Set Red LED current to 15mA
    MAX30102_Write(0x0D, 0x0F); // Set IR LED current to 15mA
}

// function to check FIFO data ready status bit(bit 0) of FIFO status register(0x03)

int MAX30102_FIFO_DataReady(void)
{
    uint8_t status = MAX30102_Read(0x03); // Read FIFO Status register (0x03)

    return (status & FIFO_READY_MASK); // Check if FIFO_DATA_READY bit is set (bit 0)
}

// Read Data from MAX30102

/*MAX30102 stores sensor data (heart rate and SpO2) in a FIFO buffer. You can read the sensor's FIFO data registers to obtain the pulse and oxygen levels.*/

// Read 6 bytes of data from the FIFO buffer (for heart rate and SpO2)

void MAX30102_ReadFIFO(uint8_t *buffer)
{
    int i;
    I2C0_Start();
    I2C0_Write(MAX30102_ADDR << 1);       // Send address with write bit (0)
    I2C0_Write(0x07);                     // FIFO data register address
    I2C0_Start();                         // Restart condition
    I2C0_Write((MAX30102_ADDR << 1) | 1); // Send address with read bit (1)

    // Read 6 bytes from FIFO data
    for (i = 0; i < 6; i++)
    {
        buffer[i] = I2C0_Read();
    }

    I2C0_Stop();
}

// Processing Raw Data
//-Filtering PPG signal to remove noise
//-Peak detection to identify heartbeats and calculate heart rate
//-SpO2 calculation based on the ratio of red and infrared light absorption

// Moving average filter function for the PPG signal
int32_t moving_average_filter(int32_t new_sample)
{
    static int32_t buffer[FILTER_SIZE] = {0};
    static uint8_t idx = 0;
    static int32_t sum = 0;

    // Subtract the oldest sample from the sum
    sum -= buffer[idx];

    // Add the new sample to the sum
    buffer[idx] = new_sample;
    sum += buffer[idx];

    // Increment the index and wrap around if necessary
    idx = (idx + 1) % FILTER_SIZE;

    // Return the average of the buffer
    return sum / FILTER_SIZE;
}

/*moving_average_filter stores the last FILTER_SIZE samples in a buffer, and each new sample is averaged with the previous ones to reduce high-frequency noise.*/

// Peak Detection to Identify Heartbeats
/*
IBI - inter beat interval
Given sensor is sampling at 100Hz(100 samples per second)
    Heart Rate(BPM) = 60 / IBI(seconds)

Since the sampling rate is 100 samples per second, the IBI is measured in number of samples and can be converted into seconds by dividing by 100

    Heart Rate(BPM) = 60 * 100 / IBI(samples)
*/

// Function to calculate BPM based on peak times
float calculate_bpm()
{
    uint8_t i;
    uint32_t avg_ibi, total_ibi = 0;
    float bpm;

    if (peak_index < 2)
    {
        return 0; // Not enough data to calculate BPM
    }

    // Calculate the average inter-beat interval (IBI) in samples
    for (i = 1; i < peak_index; i++)
    {
        total_ibi += peak_times[i] - peak_times[i - 1];
    }

    avg_ibi = total_ibi / (peak_index - 1); // Average IBI in samples
    bpm = (60.0 * SAMPLE_RATE) / avg_ibi;   // Calculate BPM from average IBI

    return bpm;
}

// Peak detection function to identify heartbeats and store peak times
int detect_peak(int32_t sample, uint32_t current_time)
{
    static int32_t previous_sample = 0;
    static int32_t second_previous_sample = 0;

    // Check if the sample is higher than the previous two samples (local maxima)
    if (previous_sample < sample && second_previous_sample < previous_sample)
    {
        if (sample - last_peak_value > PEAK_THRESHOLD && current_time - last_peak_time > MIN_DISTANCE)
        {
            // We found a peak, store the peak value and time
            peak_times[peak_index] = current_time; // Store the time of the peak
            peak_index = (peak_index + 1) % 10;    // Circular buffer for peak times
            last_peak_time = current_time;
            last_peak_value = sample; // Update last_peak_value to the current peak sample
            return 1;                 // Peak detected
        }
    }

    // Update previous samples for next comparison
    second_previous_sample = previous_sample;
    previous_sample = sample;

    return 0; // No peak detected
}

// SP02 calculation based on ratio of Red and Infrared Light Absorption

/* Formula:
    SP02 = 100 - ((R/IR) * 100)

    R is ratio of red signal intensity to the IR signal intensity(usuall in the form of peak or average values

    IR is infrared signal intensity
*/

float calculate_spo2(int32_t red_signal, int32_t ir_signal)
{
    float ratio, spo2;
    if (ir_signal == 0)
    {
        return 0.0f; // Prevent division by zero
    }

    // Calculate the ratio of red-to-IR signals
    ratio = (float)red_signal / (float)ir_signal;

    // Simple empirical equation for SpO2 estimation
    spo2 = 110 - (25 * ratio);

    // Ensure SpO2 is within realistic bounds (0 - 100%)
    if (spo2 > 100)
    {
        spo2 = 100;
    }

    else if (spo2 < 0)
    {
        spo2 = 0;
    }

    return spo2;
}

/*
The MAX30102 stores the red and infrared (IR) data in its FIFO registers, and you can read them sequentially.

The MAX30102 stores data in 24-bit format for both the red and infrared values.

*/

/*

Understanding the MAX30102 FIFO Data:

The FIFO buffer stores red and infrared values in 24-bit wide words

The data in the FIFO register comes in pairs: first a 24-bit red sample and then a 24-bit infrared sample.

To read these, you need to read 6 bytes (3 bytes for red, 3 bytes for infrared).

*/

// To read the red component of the current sample
int32_t read_red_sample(void)
{
    int32_t red_signal;
    uint8_t buffer[6]; // To store the 6 bytes (3 red, 3 IR)

    // Wait until FIFO data is ready
    while (!MAX30102_FIFO_DataReady())
    {
        wait_ms(1); // Wait 1 ms before checking again
    }

    // Read 6 bytes from FIFO (3 for red, 3 for IR)
    MAX30102_ReadFIFO(buffer);

    // Combine the first 3 bytes into a single 24-bit red signal
    red_signal = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];

    return red_signal;
}

// To read the IR component of the current sample

int32_t read_ir_sample(void)
{
    int32_t ir_signal;
    uint8_t buffer[6]; // To store the 6 bytes (3 red, 3 IR)

    // Wait until FIFO data is ready
    while (!MAX30102_FIFO_DataReady())
    {
        wait_ms(1); // Wait 1 ms before checking again
    }

    // Read 6 bytes from FIFO (3 for red, 3 for IR)
    MAX30102_ReadFIFO(buffer);

    // Combine the next 3 bytes into a single 24-bit IR signal
    ir_signal = (buffer[3] << 16) | (buffer[4] << 8) | buffer[5];

    return ir_signal;
}

/*

How the Functions Work:
FIFO Reading: Both functions call the MAX30102_ReadFIFO function, which reads 6 bytes from the FIFO buffer (3 bytes for red and 3 bytes for infrared).

Combining the Data: The MAX30102_ReadFIFO function returns a 24-bit value for each of the red and infrared components by combining 3 bytes of data.

Return Value: The read_red_sample() and read_ir_sample() functions return the 24-bit red and infrared signals, respectively.

*/

// Assume P0.19 for RS
// P0.20 for Enable
// P0.18 to P0.15 for input lines

int temp1, temp2, flag1, flag2;

void port_write()
{
    int i;
    LPC_GPIO0->FIOPIN = temp2 << 15;

    if (flag1 == 0)
        LPC_GPIO0->FIOCLR = 1 << 19;
    else
        LPC_GPIO0->FIOSET = 1 << 19;

    LPC_GPIO0->FIOSET = 1 << 20;
    for (i = 0; i < 50; i++)
        ;
    LPC_GPIO0->FIOCLR = 1 << 20;

    for (i = 0; i < 30000; i++)
        ;
}

void LCD_write()
{
    if ((flag1 == 0) & ((temp1 == 0x30) || (temp1 == 0x20)))
        flag2 = 1;
    else
        flag2 = 0;
    // flag2 = (flag1 == 1) ? 0 :((temp1 == 0x30) || (temp1 == 0x20)) ? 1 : 0;
    temp2 = temp1 >> 4;
    port_write();

    if (flag2 == 0)
    {
        temp2 = temp1 & 0xF;
        port_write();
    }
}

int main()
{
    int32_t red_sample = 0;
    int32_t ir_sample = 0;
    int32_t filtered_red = 0;
    int32_t filtered_ir = 0;
    uint32_t current_time = 0;

    float spo2, bpm;
    char dval[50], val[50];
    int i;
    char l1[] = "BPM: ";
    char l2[] = "SpO2: ";
    int lcd_init[] = {0x30, 0x30, 0x30, 0x20, 0x28, 0x0C, 0x01, 0x80, 0x06};

    SystemInit();
    SystemCoreClockUpdate();

    LPC_PINCON->PINSEL0 = 0;
    LPC_GPIO0->FIODIR = 0xF << 15 | 1 << 19 | 1 << 20;

    flag1 = 0;
    for (i = 0; i <= 8; i++)
    {
        temp1 = lcd_init[i];
        LCD_write();
    }

    // line 1 writing
    flag1 = 0;
    temp1 = 0x80;
    LCD_write();
    flag1 = 1;
    for (i = 0; l1[i] != '\0'; i++)
    {
        temp1 = l1[i];
        LCD_write();
    }

    // line 2 writing
    flag1 = 0;
    temp1 = 0xC0;
    LCD_write();
    flag1 = 1;
    for (i = 0; l2[i] != '\0'; i++)
    {
        temp1 = l2[i];
        LCD_write();
    }

    // Initialize MAX30102 and I2C
    I2C0_Init();
    MAX30102_Init();

    while (1)
    {
        // Read new samples from MAX30102 FIFO registers
        red_sample = read_red_sample(); // Red reading function
        ir_sample = read_ir_sample();   // IR reading function

        // Apply moving average filter
        filtered_red = moving_average_filter(red_sample);
        filtered_ir = moving_average_filter(ir_sample);

        if (!(detect_peak(filtered_red, current_time))) // Use red signal for heartbeat detection
        {
            // If a peak is detected, you can calculate BPM based on the peaks
            bpm = calculate_bpm();
            sprintf(dval, "%.2f", bpm);

            // Display BPM on LCD
            flag1 = 0;
            temp1 = 0x84;
            LCD_write();
            flag1 = 1;
            dval[0] = 'O';
            dval[1] = 'N';
            dval[2] = '\0';
            for (i = 0; dval[i] != '\0'; i++)
            {
                temp1 = dval[i];
                LCD_write();
            }
        }

        else
        {
            // Display BPM on LCD
            flag1 = 0;
            temp1 = 0x84;
            LCD_write();
            flag1 = 1;
            dval[0] = 'O';
            dval[1] = 'F';
            dval[2] = 'F';
            dval[3] = '\0';
            for (i = 0; dval[i] != '\0'; i++)
            {
                temp1 = dval[i];
                LCD_write();
            }
        }

        // Calculate SpO2 based on filtered samples
        spo2 = calculate_spo2(filtered_red, filtered_ir);
        sprintf(val, "%.2f%%", spo2);

        flag1 = 0;
        temp1 = 0xC5;
        LCD_write();
        flag1 = 1;
        for (i = 0; val[i] != '\0'; i++)
        {
            temp1 = val[i];
            LCD_write();
        }
        current_time++;

        // Delay or wait for the next sample (adjust as needed)
        wait_ms(10); // Example: wait 10ms for 100 Hz sample rate
    }
}
