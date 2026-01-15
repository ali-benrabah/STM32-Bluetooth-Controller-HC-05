/* Embedded Systems - Exercise 13 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

////////
void handle_tim4(void) {
    if (TIM4_SR & TIM_UIF) {     // Le timer a fini ?
        TIM4_SR = 0;             // OKK
        ADC1_CR2 |= ADC_SWSTART; // alors ADC, GO 
    }
}
/////////////////////
void handle_adc(void) {
    if (ADC1_SR & ADC_EOC) {          // L’ADC a fini ?
        ADC1_SR = 0;                  // "OK j’ai vu"
        uint16_t val = ADC1_DR;       // Je prends la valeur

        printf("Lumière : %4u  ", val);
        if (val < 2000) {
            GPIOD_ODR |=  (1 << 12);  // LED VERTE ON
            printf("→ TROP SOMBRE !\n");
        } else {
            GPIOD_ODR &= ~(1 << 12);  // LED OFF
            printf("-> OK\n");
        }
    }
}
/////////////////////

int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM3EN;
	RCC_APB1ENR |= RCC_TIM4EN; // allumer la clock du TIM4
	RCC_APB2ENR |= RCC_ADC1EN;
	//////
	GPIOA_MODER  |= (3 << 6);     // PA3 analog
    GPIOD_MODER  |= (1 << 24);    // PD12 sortie
	///
	ADC1_SQR3     = 3;
    ADC1_CR2     |= ADC_ADON;
	// initialization
	//84 MHz → 10 kHz (tick = 100 µs) */
	 // 84 000 000 / 8400 = 10 000 Hz
	TIM4_PSC = 8400 -1; // 
	TIM4_ARR = 1000;// 100ms = 1000 ticks
	TIM4_DIER |= TIM_UIE;
	TIM4_CR1 |= TIM_CEN;
	/* 1. Autorise l’interruption dans le NVIC */
	NVIC_ISER(0) |= (1 << 30);          // TIM4_IRQn = 30

	/* 2. Donne l’adresse de ta fonction */
	NVIC_IRQ(30) = (uint32_t)handle_tim4;

	ADC1_CR1 |= ADC_EOCIE;              // Active l’interruption ADC

	NVIC_IRQ(18) = (uint32_t)handle_adc;   // ADC_IRQn = 18
	NVIC_ISER(0) |= (1 << 18);

	// main loop
	printf("Endless loop!\n");
	while(1) {
	}

}


