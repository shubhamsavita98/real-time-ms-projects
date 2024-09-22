/***************************************************************************
* File Name:     main.c
* Description:   This program implements two FreeRTOS tasks: 
*                one for sending data to a queue and another for receiving 
*                data from it. The tasks are responsible for toggling 
*                LEDs based on queue operations. The sender toggles the 
*                green LED upon successfully sending to the queue, while 
*                the receiver toggles the red LED when data is received. 
*                The blue LED is toggled if no data is received within a 
*                specified timeout.
* Author:        Shubham Kumar Savita
* References:    Embedded Systems Dr. Yifeng Zue
****************************************************************************/

#include <stdio.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "FreeRTOS.h"
#include "task.h"
#include "system_clock_80MHz.h"
#include "led.h"
#include "queue.h" 

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

void senderTask (void *argument);
void receiverTask (void *argument);

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------

// Queue handle variable to store the queue
QueueHandle_t xQueue;


int main (void)
{
    // Configuration
    system_clock_80MHz();
    led_gpio_config(GREEN);
    led_gpio_config(RED);
	  led_gpio_config(BLUE);

		//create Queue
		xQueue = xQueueCreate(10, sizeof(int32_t));
	
    // Initialization
    SystemCoreClockUpdate();          // Required for FreeRTOS to know the
                                      //   system clock frequency
    led_off(GREEN);
    led_off(RED);
	  led_off(BLUE);

    // Create a separate thread for each of the two LEDs.
    // Then start the scheduler to begin executing the threads.

    xTaskCreate(senderTask,"GREEN LED", 100, (void *)100, 1, NULL);
    xTaskCreate(receiverTask,"RED LED", 100, NULL, 1, NULL);
    vTaskStartScheduler();
    
    while(1);
}

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------
void senderTask (void *argument)
{
	BaseType_t xStatus;

    for (;;)
    {
        // The 'argument' is not used; hardcoded parameters((void *)100) are currently passed.
        // Busy looping occurs if the queue is full; 
			  // consider using a timeout with pdMS_TO_TICKS(1000) instead of 0.
        xStatus = xQueueSendToBack(xQueue, &argument, 0); 
        
        if (xStatus == pdPASS)
        {
            led_toggle(GREEN);  // Toggle the green LED on successful queue send.
            vTaskDelay(pdMS_TO_TICKS(1000));  // Delay task for 1000 ms.
        }
    }
}

void receiverTask (void *argument)
{
    BaseType_t xStatus;
    
    for (;;)
    {
        // Busy looping avoided with a 100 ms wait time in xQueueReceive().
        // Adjust the third argument (timeout) to control the delay.
        xStatus = xQueueReceive(xQueue, &argument, pdMS_TO_TICKS(100)); 
        
        if (xStatus == pdPASS)
        {
            led_toggle(RED);  // Toggle the red LED on successful queue receive.
        }
        else
        {
            led_toggle(BLUE);  // Toggle the blue LED if queue is empty after timeout.
        }
    }
}
