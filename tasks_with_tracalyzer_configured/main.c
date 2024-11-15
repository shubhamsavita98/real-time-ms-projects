#include <stdio.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "FreeRTOS.h"
#include "task.h"
#include "system_clock_80MHz.h"
#include "led.h"
#include "trcRecorder.h"  // part of tracealyzer setup

//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

void green_led_blink (void *argument);
void red_led_blink (void *argument);

//------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------

/**
 * @brief   After initialization, two threads are executed using a round-robin
 *          scheduling algorithm, and so appear to run in parallel.
 */

int main (void)
 {
      
      //DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN; 
    // Configuration
    //system_clock_80MHz();
     
    led_gpio_config(GREEN);
    led_gpio_config(RED);
    xTraceEnable(TRC_START);
    //xTraceInitialize()  //optional: pairs with xTraceEnable()
    // Initialization
    SystemCoreClockUpdate();          // Required for FreeRTOS to know the
                                      //   system clock frequency
    led_off(GREEN);
    led_off(RED);

    // Create a separate thread for each of the two LEDs.
    // Then start the scheduler to begin executing the threads.

    xTaskCreate(green_led_blink,"GREEN_LED_Task", 100, NULL, 1, NULL);
    xTaskCreate(red_led_blink,"RED_LED_Task", 100, NULL, 1, NULL);
  
    vTaskStartScheduler();
    // xTraceEnable() ///optional: pairs with xTraceInitialize()
    while(1);
}

//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------

/**
 * @brief   Thread 1: Toggle the green LED every 0.5 seconds.
 */

void green_led_blink (void *argument)
{
    for (;;) {
        led_toggle(GREEN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief   Thread 2: Toggle the red LED every 1.0 second.
 */


void red_led_blink (void *argument)
{
    for (;;) {
        led_toggle(RED);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


