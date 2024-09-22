/***************************************************************************
* File Name:     main.c
* Description:   Implementation of single task toggling LED in freeRTOS that 
								 uses queue.
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
                                      // system clock frequency
    led_off(GREEN);
    led_off(RED);
	  led_off(BLUE);

    // Create a thread for the LED.
    // Then start the scheduler to begin executing the thread.
    xTaskCreate(senderTask,"GREEN LED", 100, (void *)100, 1, NULL);
    vTaskStartScheduler();
    
    while(1);
}

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------
void senderTask (void *argument)
{
	BaseType_t xStatus;

    for (;;) {
			  //busy looping when queue is full;
	      xStatus = xQueueSendToBack( xQueue, &argument, 0); 
			  if(xStatus == pdPASS){;
								led_toggle(GREEN);
								vTaskDelay(pdMS_TO_TICKS(1000));
				}
				else{
					led_on(BLUE);
				}
    }
}


