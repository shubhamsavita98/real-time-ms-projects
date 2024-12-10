#include "stm32l476xx.h"


void config_button_pin(){
	
	// Enable GPIO Port clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		
	// Enable mode
	GPIOA->MODER &= ~GPIO_MODER_MODE0;
	
	// Enable push-pull, No-pull up pull down
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;
		
}

void config_EXTI(void){
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
	EXTI->IMR1 |= EXTI_IMR1_IM0;
	EXTI->RTSR1 |= EXTI_RTSR1_RT0;  //Rising trigger selection register (RTSR):0 = disabled, 1 = enabled
}

