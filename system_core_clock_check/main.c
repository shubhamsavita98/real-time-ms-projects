#include "stm32l476xx.h"
#include "uart.h"
#include "stdio.h"
#include <stdint.h>

void display(char* message);

// print to the terminal
void display(char* message){

		int i = 0;
		while(message[i] != '\0'){
			while(!(USART2->ISR & USART_ISR_TXE));
			USART2->TDR = message[i++];			
		}
}

int main(void){
	
		// configure and setup uart
		USART2_Init();
	
		/*
		* updates SystemCoreClock variable if changed during runtime,
		* otherwise returns default value i.e. 4MHz.
	  */
		SystemCoreClockUpdate();
	
		uint32_t sys_clk = SystemCoreClock;
	  char clk[50];
	
		char message[] = "System Clock Frequency\n\r";
	  display(message);
	
	  sprintf(clk, "%u Hz\n\r", sys_clk);
		display(clk);
	
		while(1);
}

