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
//#include "system_clock_80MHz.h"


// Constants
#define QUEUE_LENGTH 1
#define TEMP_BUFFER_LENGTH 25 
#define QUEUE_ITEM_SIZE sizeof(uint32_t)

// Global variables
char tempC_buffer[TEMP_BUFFER_LENGTH]; // temperature buffer
uint32_t temperature_C; // temperature in Celsius
float voltage_raw; // raw voltage value from sensor
float voltage; // voltage in mV (stored after conversion)

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------
void send_string_via_usart(const char *str);
void sensor_acquisition(void *argument);
void data_processing(void *argument);
//void uart_logging(void *argument);
void button_task(void *argument);

//------------------------------------------------------------------------------
// Task handles 
//------------------------------------------------------------------------------
TaskHandle_t SensorTaskHandle;
TaskHandle_t ProcessingTaskHandle;
TaskHandle_t ButtonTaskHandle;
//TaskHandle_t UartTaskHandle;
//------------------------------------------------------------------------------
// Queue handles 
//------------------------------------------------------------------------------
QueueHandle_t temperatureQ;

//------------------------------------------------------------------------------
// Semaphore handle :: code implements binary semaphore
//------------------------------------------------------------------------------
SemaphoreHandle_t ButtonSemaphore;

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------
/**
 * @brief   freeRTOS based temperature data acquisition system.
 *          
 */

int main (void)
 {
  
		// Configuration
		//system_clock_80MHz();
	 	config_button_pin();
		config_EXTI();
		led_gpio_config();
	 
		// Initialization
		SystemCoreClockUpdate(); // Required for FreeRTOS to know the system clock frequency
	  // Initialize ADC
		ADC_Init();
		// Initialize UART
		USART2_Init();

		// Only enable tracing in debug mode to reduce RAM usage in standalone mode 
		if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
			xTraceEnable(TRC_START);
		}
		//xTraceInitialize()  //optional: pairs with xTraceEnable() 
		 
	  led_off(); // led initial state
		send_string_via_usart("Temperature Sensor Initialized\n\r");

		temperatureQ = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
		// Separate tasks for each processes
		xTaskCreate(sensor_acquisition,"Sensor Acquisition Task", 100, NULL, 1, &SensorTaskHandle);
		xTaskCreate(data_processing,"Data Processing Task", 100, NULL, 2, &ProcessingTaskHandle);
		xTaskCreate(button_task,"Button Task", 100, NULL, 3, &ButtonTaskHandle);
		//xTaskCreate(uart_logging,"UART Logging Task", 100, NULL, 1, &UartTaskHandle);
		ButtonSemaphore = xSemaphoreCreateBinary();

		// Start the Scheduler
		vTaskStartScheduler();
		// xTraceEnable() ///optional: pairs with xTraceInitialize()

		while(1);
}

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------

// Send string over UART ::: NEED TO BE GUARDED IF NO QUEUE
void send_string_via_usart(const char *str) {
  while (*str) {
      while (!(USART2->ISR & USART_ISR_TXE)); // Wait until TXE (Transmit Data Register Empty)
      USART2->TDR = *str++; // Load the next character into TDR
  }
}


/**
 * @brief   Task 1: Sensor data acquisition.
 */
void sensor_acquisition(void *argument){
  for (;;) {
    // Start ADC conversion
		ADC1->CR |= ADC_CR_ADSTART;
		
		voltage_raw = adc_result; 
		if(!xQueueSend(temperatureQ, &voltage_raw, portMAX_DELAY)){
			  send_string_via_usart("Failed to send data to Queue\n\r");
		}
		// Stop ADC conversion
		ADC1->CR &= ~ADC_CR_ADSTART;
		vTaskDelay(pdMS_TO_TICKS(100)); // Wait before next read
  }
}


/**
 * @brief   Task 2: processing the sensor data.
 */
void data_processing(void *argument){
  for (;;) {
		float voltage_data_received = 0;
    // Receive data for processing
		if(!xQueueReceive(temperatureQ, &voltage_data_received, portMAX_DELAY)){
				send_string_via_usart("Failed to receive send data from Queue\n\r");
		}
		else{
			// Calculate temperature
			voltage = (0.00081 * voltage_data_received);
			temperature_C = (voltage - 0.5)*100;
			//format the temperature and send over UART
			sprintf(tempC_buffer, "Temperature: %u C\n\r", temperature_C);
			send_string_via_usart(tempC_buffer);	
		}
  }
}


/**
 * @brief   Task 3: logging to the UART.
 */
void button_task(void *argument){	
	for(;;){
		/* Wait for notification from ISR */
		xSemaphoreTake(ButtonSemaphore, portMAX_DELAY);
		led_on();
    send_string_via_usart("Button Pressed\n\r");
    led_off();		
	}
}


void EXTI0_IRQHandler(void) {
	led_on();
	// PR (Pending Register): Check if the interrupt is triggered by EXTI13, as EXTI 10-15 share this interrupt vector.	
	if ((EXTI->PR1 & EXTI_PR1_PIF0) == EXTI_PR1_PIF0) {
		// cleared by writing a 1 to this bit
		EXTI->PR1 |= EXTI_PR1_PIF0;	
		BaseType_t priorityStatus = pdFALSE; // by default false; low priority, no context switch
		xSemaphoreGiveFromISR(ButtonSemaphore, &priorityStatus);
		portYIELD_FROM_ISR(priorityStatus); // if high priority task then context switch; status becomes pdTRUE
	}
	led_on();
}

/**
 * @brief   Task 3: logging to the UART.
 */
/* TODO ::
void uart_logging(void *argument){
	
	for(;;){
		//TODO : with two queues 
		vTaskDelay(pdMS_TO_TICKS(100));		
	}
}
*/


