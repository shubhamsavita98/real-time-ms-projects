
#include "led.h"


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
				
				case BLUE:
        default:
            // Enable the peripheral clock for GPIO port A.
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

            // Set pin PA2 to digital output: 01.
            GPIOB->MODER &= ~GPIO_MODER_MODE3;
            GPIOB->MODER |= GPIO_MODER_MODE3_0;

            // Set output type of pin PA2 to push-pull: 0 (default).
            GPIOB->OTYPER &= ~GPIO_OTYPER_OT3;

            // Set pin PA5 to low speed output: 00.
            GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED3;

            // Set pin PA5 to no pull-up, no pull-down: 00.
            GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD3;
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
            GPIOA->ODR &= ~GPIO_ODR_OD5;
            break;
				
				case BLUE:
        default:
            GPIOB->ODR &= ~GPIO_ODR_OD3;
            break;
    }
}

void led_on(uint8_t color)
{
    switch (color) {
        case RED:
            GPIOA->ODR |= GPIO_ODR_OD6;
            break;

        case GREEN:
            GPIOA->ODR |= GPIO_ODR_OD5;
            break;
				
				case BLUE:
        //default:
            GPIOB->ODR |= GPIO_ODR_OD3;
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
            GPIOA->ODR ^= GPIO_ODR_OD5;
            break;
				
				case BLUE:
        //default:
            GPIOB->ODR ^= GPIO_ODR_OD3;
            break;
    }
}


