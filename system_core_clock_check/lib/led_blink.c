/**
 * @author  L. Markowsky
 * @date    24-Apr-2023
 * @file    main.c
 * @version V1.0.1
 *
 * @brief   This program demonstrates how to use FreeRTOS (without the CMSIS
 *          interface) on the Nucleo-L476RG with the following contraints:
 *
 *          - The program must use the Keil IDE.
 *          - The program must compile with the Arm Compiler (version 6).
 *          - The program must use an 80 MHz system clock.
 *
 *          Two tasks are created:
 *          - Task 1: Toggle GPIO pin PA5 every 0.5 seconds.
 *                    Pin PA5 drives the green user LED on the Nucleo board.
 *                    A separate green LED may be placed on a breadboard in
 *                    series with a 680 Ohm resistor as well.
 *
 *          - Task 2: Toggle GPIO pin PA6 every 1.0 second.
 *                    A red LED may be placed on a breadboard in series with
 *                    a 680 Ohm resistor to view the output.
 *
 *          After initialization, two threads are executed using a round-robin
 *          scheduling algorithm, and so the two tasks appear to run in parallel.
 *
 * @note    References used:
 *
 *          - Embedded Systems with ARM Cortex-M Microcontrollers in Assembly
 *            Language and C, 4th Edition, by Dr. Yifeng Zhu, January 2023,
 *            (Chapter 20: Multitasking, pages 591-610).
 *
 *          - Getting Started with STM32 - Introduction to FreeRTOS (Video and
 *            web page), by Shawn Hymel (Digi-Key Electronics) [online
 *            https://www.digikey.com/en/maker/projects/getting-started-with-stm32-introduction-to-freertos/ad275395687e4d85935351e16ec575b1,
 *            accessed 22 Apr 2023].
 *
 *          - FreeRTOS: Real-Time Operating System for Microcontrollers,
 *            Amazon Web Services, Inc. 
 *            [online https://www.freertos.org/index.html, accessed 21 Apr 2023].
 *
 *          - CMSIS-FreeRTOS Version 10.5.2-dev3: CMSIS-RTOS2 Compliant FreeRTOS
 *            Implementation, by Arm Limited, 2023, (Section: Example Projects)
 *            [available online
 *            https://arm-software.github.io/CMSIS-FreeRTOS/main/examples.html,
 *            accessed 12 Apr 2023].
 *
 *          - Mastering the FreeRTOS Real Time Kernel: A Hands-On Tutorial Guide,
 *            by Richard Barry, Pre-release 161204 Edition, 2016 (Section 1.4
 *            Creating a FreeRTOS Project, Adapting One of the Supplied Demo
 *            Projects, page 18) [available online
 *            https://www.freertos.org/fr-content-src/uploads/2018/07/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf,
 *            accessed 23 Apr 2023].
 *
 *          - Beginning STM32: Developing with FreeRTOS, libopencm3, and GCC,
 *            by Warren Gay, Apress 2018, (Chapter 5: FreeRTOS, pages 59-72).
 */

#include <stdio.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "FreeRTOS.h"
#include "task.h"
#include "system_clock_80MHz.h"
#include "led.h"

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
    // Configuration
    system_clock_80MHz();
    led_gpio_config(GREEN);
    led_gpio_config(RED);

    // Initialization
    SystemCoreClockUpdate();          // Required for FreeRTOS to know the
                                      //   system clock frequency
    led_off(GREEN);
    led_off(RED);

    // Create a separate thread for each of the two LEDs.
    // Then start the scheduler to begin executing the threads.

    xTaskCreate(green_led_blink,"GREEN LED", 100, NULL, 15, NULL);
    xTaskCreate(red_led_blink,"RED LED", 100, NULL, 15, NULL);
    vTaskStartScheduler();
    
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

