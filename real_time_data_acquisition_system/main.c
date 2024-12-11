#include <stdio.h>
#include "string.h"
#include "sensor_ADC_driver.h"
#include "usart2_driver.h"
#include "button.h"
#include "led.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "trcRecorder.h"

// Constants
#define QUEUE_LENGTH 1
#define TEMP_BUFFER_LENGTH 25 
#define QUEUE_ITEM_SIZE sizeof(uint32_t)

// Global variables
uint32_t temperature_C; // temperature in Celsius
float voltage_raw; // raw voltage value from sensor
float voltage; // voltage in mV (stored after conversion)

// Current mode (0 - Idle, 1 - Monitor, 2 - Log)
volatile uint8_t current_mode = 0;

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
void send_string_via_usart(const char *str);
void sensor_acquisition(void *argument);
void data_processing(void *argument);
void button_task(void *argument);
void uart_logging(void *argument);

//------------------------------------------------------------------------------
// Task handles 
//------------------------------------------------------------------------------
TaskHandle_t SensorTaskHandle;
TaskHandle_t ProcessingTaskHandle;
TaskHandle_t ButtonTaskHandle;
TaskHandle_t UartTaskHandle;

//------------------------------------------------------------------------------
// Queue handles 
//------------------------------------------------------------------------------
QueueHandle_t temperatureQ;
QueueHandle_t uartQ;

//------------------------------------------------------------------------------
// Semaphore and mutex handle  :: code implements binary semaphore
//------------------------------------------------------------------------------
SemaphoreHandle_t ButtonSemaphore;

/**
 * @brief   freeRTOS based temperature data acquisition system.
 */
int main (void)
{
    // Configuration
    config_button_pin();
    config_EXTI();
    led_gpio_config();

    // Initializations
    ADC_Init();
    USART2_Init();
    SystemCoreClockUpdate();  // Required for FreeRTOS to know the system clock frequency

    // Only enable tracing in debug mode to reduce RAM usage in standalone mode 
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
        xTraceEnable(TRC_START);
    }

    led_off(); // LED initial state

    temperatureQ = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    uartQ = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

    // Create tasks
    xTaskCreate(sensor_acquisition, "Sensor Acquisition Task", 100, NULL, 1, &SensorTaskHandle);
    xTaskCreate(data_processing, "Data Processing Task", 100, NULL, 1, &ProcessingTaskHandle);
    xTaskCreate(button_task, "Button Task", 100, NULL, 1, &ButtonTaskHandle);
    xTaskCreate(uart_logging, "UART Logging Task", 200, NULL, 3, &UartTaskHandle);

    ButtonSemaphore = xSemaphoreCreateBinary();

    // Start the Scheduler
    vTaskStartScheduler();

    while(1);
}

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------

// Send string over UART
void send_string_via_usart(const char *str) {
    while (*str) {
        while (!(USART2->ISR & USART_ISR_TXE)); // Wait until TXE (Transmit Data Register Empty)
        USART2->TDR = *str++; // Load the next character into TDR
    }
}

// Task 1: Sensor data acquisition.
void sensor_acquisition(void *argument) {
    for (;;) {
        if (current_mode == 1 || current_mode == 2) {  // Monitor or Log mode
					led_on();
            // Start ADC conversion
            ADC1->CR |= ADC_CR_ADSTART;

            voltage_raw = adc_result;
            if (!xQueueSend(temperatureQ, &voltage_raw, portMAX_DELAY)) {
                const char *error_msg = "Failed to send data to Queue\n\r";
                xQueueSend(uartQ, &error_msg, portMAX_DELAY);
            }

            // Stop ADC conversion
            ADC1->CR &= ~ADC_CR_ADSTART;
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Wait before next read
    }
}

// Task 2: Processing the sensor data.
void data_processing(void *argument) {
    for (;;) {
        float voltage_data_received = 0;
        // Receive data for processing
        if (!xQueueReceive(temperatureQ, &voltage_data_received, portMAX_DELAY)) {
            const char *error_msg = "Failed to receive data from Queue\n\r";
            xQueueSend(uartQ, &error_msg, portMAX_DELAY);
        } else {
            // Calculate temperature
            voltage = (0.00081 * voltage_data_received);
            temperature_C = (voltage - 0.5) * 100;

            // Format the temperature message
            char *temp_msg = pvPortMalloc(TEMP_BUFFER_LENGTH * sizeof(char));
            if (temp_msg) {
                snprintf(temp_msg, TEMP_BUFFER_LENGTH, "Temperature: %u C\n\r", temperature_C);
                xQueueSend(uartQ, &temp_msg, portMAX_DELAY); // Send pointer to queue
            } else {
                const char *error_msg = "Memory allocation failed for temp_msg\n\r";
                xQueueSend(uartQ, &error_msg, portMAX_DELAY);
            }
        }
				led_off();
    }
}

// Task 3: Button task to switch modes.
void button_task(void *argument) {
    for (;;) {
        /* Wait for notification from ISR */
        xSemaphoreTake(ButtonSemaphore, portMAX_DELAY);

        // Cycle through modes on each button press
        current_mode = (current_mode + 1) % 3;  // Cycle through modes: 0, 1, 2

        // Indicate the mode change via UART
        const char *mode_msg;
        switch (current_mode) {
            case 0:
                mode_msg = "Mode: Idle\n\r";
                led_off();
                break;
            case 1:
                mode_msg = "Mode: Monitor\n\r";
                led_on();
                break;
            case 2:
                mode_msg = "Mode: Log\n\r";
                led_on();
                break;
            default:
                mode_msg = "Unknown Mode\n\r";
                break;
        }
        xQueueSend(uartQ, &mode_msg, portMAX_DELAY);
    }
}

// Interrupt Handler for EXTI0 (button press)
void EXTI0_IRQHandler(void) {
    if ((EXTI->PR1 & EXTI_PR1_PIF0) == EXTI_PR1_PIF0) {
        EXTI->PR1 |= EXTI_PR1_PIF0;  // Clear interrupt flag
        BaseType_t priorityStatus = pdFALSE;
        xSemaphoreGiveFromISR(ButtonSemaphore, &priorityStatus);
        portYIELD_FROM_ISR(priorityStatus); // Trigger context switch if needed
    }
}

// Task 4: UART logging task.
void uart_logging(void *argument) {
    for (;;) {
        char *uart_msg;
        // Dequeue messages and send via UART
        if (xQueueReceive(uartQ, &uart_msg, portMAX_DELAY)) {
            send_string_via_usart(uart_msg);
            vPortFree(uart_msg); // Free memory after use
        }
    }
}
