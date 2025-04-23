#include "stdio.h"
#include "stm32l476xx.h" 
#include "ADC.h"
#include "LED.h"
#include "SysTimer.h"
#include "SysClock.h"

#define LED_PIN    5
#define TIMING_PIN 6

volatile uint32_t result[256];		// Declare an array of 256 data points

void turn_on_LED(){
	GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED(){
	GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED(){
	GPIOA->ODR ^= (1 << LED_PIN);
}

void turn_on_TIMING(){
	GPIOA->ODR |= 1 << TIMING_PIN;
}

void turn_off_TIMING(){
	GPIOA->ODR &= ~(1 << TIMING_PIN);
}

void toggle_TIMING(){
	GPIOA->ODR ^= (1 << TIMING_PIN);
}

int main(void){
	
	int i;

	System_Clock_Init(); 	// Switch System Clock = 80 MHz
	
	LED_Init();						// Set up the pin for the built in Green LED PA(5)
	TIMING_Init();				// Set up Pin PA(6) as an auxiliary timing pin to be monitored with a scope
	
	
	// Analog Inputs: 
	//  PA1 (ADC12_IN6)
	//  0V <=> 0, 3.0V <=> 4095
	ADC_Init();						// Set up PA(1) as the Analog pin for reading voltage input from the potentiometer
	turn_off_LED();
	turn_off_TIMING();
	i=0;									// Initialize Result array index
	
	while(1){
		
		ADC1->CR |= ADC_CR_ADSTART;				// Start the conversion
		turn_on_TIMING();
		while ( (ADC123_COMMON->CSR & ADC_CSR_EOC_MST) == 0);		// Wait for conversion done
		turn_off_TIMING();
		turn_on_LED();
		turn_off_LED();
		
		result[(i++ & 0x00FF)] = ADC1->DR;											// Index array and store reading
		
		
	}
}
