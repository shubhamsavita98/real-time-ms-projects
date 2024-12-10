#include <stdio.h>
#include "string.h"
#include "sensor_ADC_driver.h"
#include "usart2_driver.h"
#include "button.h"
#include "led.h"
//#include "trcRecorder.h"


// Constants
#define TEMP_BUFFER_LENGTH 25 

// Global variables
char tempC_buffer[TEMP_BUFFER_LENGTH]; // Temperature buffer
uint32_t temperature_C; // Temperature in Celsius
float voltage_raw; // Raw voltage value from sensor
float voltage; // Voltage in mV (stored after conversion)
volatile uint8_t button_pressed = 0; // Flag for button press

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
void send_string_via_usart(const char *str);
void process_sensor_data(void);
void handle_button_press(void);
void EXTI0_IRQHandler(void);

int main(void) {
    // Configuration
    config_button_pin();
    config_EXTI();
    led_gpio_config();
    
    // Initialization
    SystemCoreClockUpdate();
    ADC_Init();
    USART2_Init();
		
		//vTraceEnable(TRC_START);
	
    led_off(); // LED initial state
    send_string_via_usart("Temperature Sensor Initialized\n\r");

    while (1) {
			  
        // Sensor acquisition
        ADC1->CR |= ADC_CR_ADSTART; // Start ADC conversion
        voltage_raw = adc_result; // Capture ADC result
        ADC1->CR &= ~ADC_CR_ADSTART; // Stop ADC conversion

        // Process sensor data
        process_sensor_data();

        // Handle button press if detected
        if (button_pressed) {
            handle_button_press();
            button_pressed = 0; // Reset flag
        }

        // Simulate periodic delay
        for (volatile int i = 0; i < 10000; i++);
    }
}

void send_string_via_usart(const char *str) {
    while (*str) {
        while (!(USART2->ISR & USART_ISR_TXE)); // Wait until TXE (Transmit Data Register Empty)
        USART2->TDR = *str++; // Load the next character into TDR
    }
}

void process_sensor_data(void) {
    voltage = (0.00081 * voltage_raw);
    temperature_C = (voltage - 0.5) * 100;
    sprintf(tempC_buffer, "Temperature: %u C\n\r", temperature_C);
    send_string_via_usart(tempC_buffer);
}

void handle_button_press(void) {
		led_on();
    send_string_via_usart("Button Pressed\n\r");
    led_off();
}

void EXTI0_IRQHandler(void) {
		led_on(); // indicator for interrupt start
    if ((EXTI->PR1 & EXTI_PR1_PIF0) == EXTI_PR1_PIF0) {
        EXTI->PR1 |= EXTI_PR1_PIF0; // Clear interrupt flag
        button_pressed = 1; // Set flag for button press       
    }
		led_off(); // indicator for interrupt stop
}
