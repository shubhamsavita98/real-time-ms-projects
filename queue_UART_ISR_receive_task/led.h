/**
 * @author  L. Markowsky
 * @date    24-Apr-2023
 * @file    led.h
 * @version V1.0.0
 *
 * @brief   Definitions and function prototypes for programs to use LEDs,
 *          including the Nucleo-L476RG's green user LED. The Nucleo-L476RG's
 *          green user LED is located at Port A, Pin 5 (PA5). The functions
 *          also handle a red LED at Port A, Pin 6 (PA6). Unexpected parameters
 *          default to the green user LED.
 */

#ifndef __LED_H
#define __LED_H

#include "stm32l476xx.h"

//------------------------------------------------------------------------------
// Defined Constants (to represent green LED at PA5, red LED at PA6)
//------------------------------------------------------------------------------

enum {GREEN, RED, BLUE};

//------------------------------------------------------------------------------
// Function Prototypes (in alphabetical order)
//------------------------------------------------------------------------------

void led_gpio_config(uint8_t color);
void led_off(uint8_t color);
void led_on(uint8_t color);
void led_toggle(uint8_t color);

#endif /* __LED_H */

