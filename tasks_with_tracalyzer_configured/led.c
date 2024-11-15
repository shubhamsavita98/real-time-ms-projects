/**
 * @author  L. Markowsky
 * @date    24-Apr-2023
 * @file    led.c
 * @version V1.0.0
 *
 * @brief   Functions to configure and use LEDs, including the Nucleo-L476RG's
 *          green user LED. The Nucleo-L476RG's green user LED is located at
 *          Port A, Pin 5 (PA5). The functions also handle a red LED at Port A,
 *          Pin 6 (PA6). Unexpected parameters default to the green user LED.
 *
 * @note    Reference used:
 *          - Embedded Systems with ARM Cortex-M Microcontrollers in Assembly
 *            Language and C, 3rd Ed., by Dr. Yifeng Zhu, Chap. 14 and Section
 *            15.2.2, pp. 341-372 and 379-381 (June 2018).
 */

#include "led.h"

//------------------------------------------------------------------------------
// Function Definitions (in alphabetical order)
//------------------------------------------------------------------------------

/**
 * @brief   Configure GPIO port/pin for use by the LED specified by the
 *          parameter as digital output.
 *
 *          After enabling the peripheral clock, configure the pin used
 *          by the LED as low-speed digital output with push-pull, no
 *          pull-up/pull-down.
 *
 *          The Nucleo board's green user LED is used as a 
 *          default for unexpected values of the parameter.
 *
 * @note    This function is based on functions shown on pages 356-357,
 *          Embedded Systems (ref above) by Dr. Zhu.
 */

void led_gpio_config(uint8_t color)
{
    switch (color) {
        case RED:
            // Enable the peripheral clock for GPIO port A.
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

            // Set pin PA6 to digital output: 01.
            GPIOA->MODER &= ~GPIO_MODER_MODE6;
            GPIOA->MODER |= GPIO_MODER_MODE6_0;

            // Set output type of pin PA6 to push-pull: 0 (default).
            GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;

            // Set pin PA6 to low speed output: 00.
            GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;

            // Set pin PA6 to no pull-up, no pull-down: 00.
            GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD6;
            break;

        case GREEN:
        default:
            // Enable the peripheral clock for GPIO port A.
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

            // Set pin PA5 to digital output: 01.
            GPIOA->MODER &= ~GPIO_MODER_MODE5;
            GPIOA->MODER |= GPIO_MODER_MODE5_0;

            // Set output type of pin PA5 to push-pull: 0 (default).
            GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

            // Set pin PA5 to low speed output: 00.
            GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;

            // Set pin PA5 to no pull-up, no pull-down: 00.
            GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;
            break;
    }
}

/**
 * @brief   Turn off the LED specified by the parameter.
 *          The Nucleo board's green user LED is used as a 
 *          default for unexpected values of the parameter.
 */

void led_off(uint8_t color)
{
    switch (color) {
        case RED:
            GPIOA->ODR &= ~GPIO_ODR_OD6;
            break;

        case GREEN:
        default:
            GPIOA->ODR &= ~GPIO_ODR_OD5;
            break;
    }
}

/**
 * @brief   Toggle the LED specified by the parameter.
 *          The Nucleo board's green user LED is used as a 
 *          default for unexpected values of the parameter.
 */

void led_toggle(uint8_t color)
{
    switch (color) {
        case RED:
            GPIOA->ODR ^= GPIO_ODR_OD6;
            break;

        case GREEN:
        default:
            GPIOA->ODR ^= GPIO_ODR_OD5;
            break;
    }
}


