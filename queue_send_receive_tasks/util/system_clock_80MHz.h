/**
 * @author  Prof. Yifeng Zhu
 * @date    2-February-2023
 * @file    system_clock_80MHz.h
 * @version V1.2.1
 *
 * @brief   Function declaration to configure the system clock to 80 MHz.
 *          The 16 MHz HSI is used as input to the PLL clock with scaling
 *          factors to produce an 80 MHz system clock.
 *
 * @note    This file was slightly modified by L. Markowsky. The name of
 *          the constant to prevent multiple inclusion was changed to be
 *          consistent with the board used this semester, the function and
 *          file were renamed for consistency, and this header was added.
 */

#ifndef __NUCLEO_L476RG_CLOCK_H
#define __NUCLEO_L476RG_CLOCK_H

#include "stm32l476xx.h"

void system_clock_80MHz(void);

#endif /* __NUCLEO_L476RG_CLOCK_H */

