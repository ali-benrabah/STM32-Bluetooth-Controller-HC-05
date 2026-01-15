/* Embedded Systems - Exercise 8 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0


int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	//       
	RCC_AHB1ENR |= RCC_GPIOCEN; // j ai initialisee l horloge du GPIOC mais je l'ai pas utilisee
	//

	// initialization
     // PD12 en sortie
	 GPIOD_MODER = (GPIOC_MODER & ~(0x3 << (GREEN_LED*2))) | (1 << (GREEN_LED*2));
    GPIOD_OTYPER &= ~(1 << GREEN_LED);
    GPIOD_PUPDR &= ~(0x3 << (GREEN_LED*2));
	// main loop
	printf("Endless loop!\n");
	while(1) {
		GPIOD_ODR ^= (1 << GREEN_LED);     // Toggle LED
        for (volatile int i = 0; i < 800000; i++);  // boucle approximative 500ms
	}

}


