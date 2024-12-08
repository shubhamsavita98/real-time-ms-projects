#include "led.h"


void led_gpio_config(void)
{
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

}

void led_off(void)
{
            GPIOA->ODR &= ~GPIO_ODR_OD6;

}

void led_on(void)
{
            GPIOA->ODR |= GPIO_ODR_OD6;

}
