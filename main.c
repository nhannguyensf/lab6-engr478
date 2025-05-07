#include "stdio.h"
#include "stm32l476xx.h"
#include "ADC.h"
#include "LED.h"
#include "SysTimer.h"
#include "SysClock.h"
#include "motor.h"

#define LED_PIN 5
#define TIMING_PIN 6

volatile uint32_t result[256]; // Declare an array of 256 data points
uint32_t motor_speed;
uint32_t last_val;

void turn_on_LED()
{
	GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED()
{
	GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED()
{
	GPIOA->ODR ^= (1 << LED_PIN);
}

void turn_on_TIMING()
{
	GPIOA->ODR |= 1 << TIMING_PIN;
}

void turn_off_TIMING()
{
	GPIOA->ODR &= ~(1 << TIMING_PIN);
}

void toggle_TIMING()
{
	GPIOA->ODR ^= (1 << TIMING_PIN);
}

void LED_Bar_Init()
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	// Set PB1, PB13, PB14, PB15 to output mode (MODER = 01)
	GPIOB->MODER &= ~((3U << (2 * 1)) | (3U << (2 * 13)) | (3U << (2 * 14)) | (3U << (2 * 15)));
	GPIOB->MODER |= ((1U << (2 * 1)) | (1U << (2 * 13)) | (1U << (2 * 14)) | (1U << (2 * 15)));
}

void show_LED_bar(uint32_t adc_val)
{
	uint8_t level = adc_val / 1024; // Range 0–4

	// Turn off all 4 LEDs
	GPIOB->ODR &= ~((1U << 1) | (1U << 13) | (1U << 14) | (1U << 15));

	if (level >= 0)
		GPIOB->ODR |= (1U << 1); // LED 1
	if (level >= 1)
		GPIOB->ODR |= (1U << 15); // LED 2
	if (level >= 2)
		GPIOB->ODR |= (1U << 14); // LED 3
	if (level >= 3)
		GPIOB->ODR |= (1U << 13); // LED 4
}

int main(void)
{
	int i;

	System_Clock_Init(); // Switch System Clock = 80 MHz
	initMotors();		 // PWM & direction for motors

	LED_Init();	   // Set up the pin for the built in Green LED PA(5)
	TIMING_Init(); // Set up Pin PA(6) as an auxiliary timing pin to be monitored with a scope
	LED_Bar_Init();

	// Analog Inputs:
	//  PA1 (ADC12_IN6)
	//  0V <=> 0, 3.0V <=> 4095
	ADC_Init(); // Set up PA(1) as the Analog pin for reading voltage input from the potentiometer
	turn_off_LED();
	turn_off_TIMING();
	i = 0; // Initialize Result array index

	while (1)
	{

		ADC1->CR |= ADC_CR_ADSTART; // Start the conversion
		turn_on_TIMING();
		while ((ADC123_COMMON->CSR & ADC_CSR_EOC_MST) == 0)
			; // Wait for conversion done
		turn_off_TIMING();
		// turn_on_LED();
		// turn_off_LED();

		uint32_t adc_val = ADC1->DR;
		if (adc_val > 3000)
		{
			turn_on_LED(); // PA5 green LED
		}
		else
		{
			turn_off_LED();
		}

		result[(i++ & 0x00FF)] = adc_val; // Store value in circular buffer

		// Get the most recent stored value
		last_val = result[(i - 1) & 0x00FF];

		show_LED_bar(last_val); // update LED bar

		// Map ADC (0–4095) to motor speed (0–1000)
		// motor_speed = (last_val * 1000) / 4095;
		// driveMotor(MOTOR_LEFT, motor_speed);
	}
}
