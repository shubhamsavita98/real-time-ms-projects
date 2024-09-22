/***************************************************************************
* File Name:     main.c
* Description:   This program implements a FreeRTOS-based application 
*                that utilizes USART2 for serial communication. It includes 
*                an interrupt handler for receiving UART data, which is 
*                sent to a queue for processing by a receiver task. The 
*                receiver task toggles the green LED when the character '1' 
*                is received, toggles the red LED for '2', and logs an 
*                error for invalid input. If no data is received within a 
*                specified timeout, a message is printed indicating the 
*                absence of data. The main function initializes the system 
*                clock, USART2, and the LEDs, then starts the FreeRTOS 
*                scheduler.
* Author:        Shubham Kumar Savita
* References:    FreeRTOS Documentation, STM32 Reference Manual
****************************************************************************/

#include <stdio.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "FreeRTOS.h"
#include "task.h"
#include "system_clock_80MHz.h"
#include "led.h"
#include "queue.h" 
#include "USART2.h"

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

/*****************************************************************
* Note: Sender function is going to be the interrupt handler (ISR).
*       Only the receiver task will be handled by the RTOS scheduler.
******************************************************************/
void receiverTask (void *argument);

// UART modular print function
void prints(char* message);

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------

// Queue handle for managing the data queue between the ISR and the receiver task
QueueHandle_t xQueue;

int main (void)
{
    // Configuration
    USART2_Init();  // Initialize USART2 for communication
    
    prints("System Initialized!\n\r");

    // Configure GPIO pins for LEDs
    led_gpio_config(GREEN);
    led_gpio_config(RED);
    led_gpio_config(BLUE);
1
    // Create a queue to hold up to 10 integer values (in this case, for UART data)
    xQueue = xQueueCreate(10, sizeof(int32_t));

    // Update system clock for FreeRTOS timing mechanisms
    SystemCoreClockUpdate();

    // Initialize all LEDs to OFF state
    led_off(GREEN);
    led_off(RED);
    led_off(BLUE);

    // Create receiver task and start the FreeRTOS scheduler
    xTaskCreate(receiverTask, "Receiver Task", 100, NULL, 1, NULL);
    vTaskStartScheduler();
    
    // Main loop should never be reached due to the scheduler, 
    // but exists for safety.
    while(1);
}

//------------------------------------------------------------------------------
// Interrupt Handler: USART2
//------------------------------------------------------------------------------
//
// This function handles incoming UART data via the USART2 peripheral.
// The data is passed to a FreeRTOS queue to be processed by the receiver task.
//
void USART2_IRQHandler(void)
{
    uint8_t data;

    // Check if RXNE (Receive Not Empty) interrupt is triggered
    if(USART2->ISR & USART_ISR_RXNE)
    {
        // Read the received data from USART2's receiver data register (RDR)
        data = USART2->RDR;

        // Try to send the received data to the queue from ISR
        BaseType_t yield_required = 0;
        if (!xQueueSendFromISR(xQueue, &data, &yield_required)) 
        {
            // Log message if data cannot be sent to the queue
            prints("Failed to send UART data to the queue.\n\r");
        }
        else 
        {
            // Log success message
            prints("Successfully sent data from ISR to Queue!\n\r");
        }
    }
}

//------------------------------------------------------------------------------
// FreeRTOS Task: Receiver Task
//------------------------------------------------------------------------------
//
// This function runs in the RTOS context and is responsible for
// receiving data from the queue (filled by the ISR) and handling 
// the corresponding LED toggling based on the received input.
//
void receiverTask (void *argument)
{
    BaseType_t xStatus;
    uint8_t rx_data = 0;  // Variable to hold received data
    
    for (;;)
    {
        // Receive data from the queue with a timeout of 100 ms
        // This prevents busy-waiting when the queue is empty
        xStatus = xQueueReceive(xQueue, &rx_data, pdMS_TO_TICKS(100));

        if(xStatus == pdPASS)
        {
            // Check the received data and toggle LEDs accordingly
            if(rx_data == '1')
            {
                prints("Data Received: '1' - Toggling GREEN LED.\n\r");
                led_toggle(GREEN);
            }
            else if(rx_data == '2')
            {
                prints("Data Received: '2' - Toggling RED LED.\n\r");
                led_toggle(RED);
            }
            else
            {
                // Invalid input received, log the error
                prints("Invalid input received.\n\r");
            }
        }
        else
        {
            // No data was received within the timeout, log the message
            prints("No data received from the queue.\n\r");
        }
    }
}

//------------------------------------------------------------------------------
// Utility Function: UART Print
//------------------------------------------------------------------------------
//
// This function is a simple utility to send a string of characters
// over the USART2 interface.
//
void prints(char* message)
{
    int m = 0;
    
    // Transmit each character in the string until null terminator is reached
    while(message[m] != '\0')
    {			
        // Wait until the TXE (Transmit Data Register Empty) flag is set
        while(!(USART2->ISR & USART_ISR_TXE));
        
        // Send the next character in the message
        USART2->TDR = message[m++];
    }
}
