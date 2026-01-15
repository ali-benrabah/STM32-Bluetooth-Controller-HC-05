/* Embedded Systems - Exercise 12 */

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


int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN; // allumer la clock de GPIOA
	RCC_AHB1ENR |= RCC_GPIODEN; // PD12
	RCC_APB1ENR |= RCC_TIM4EN;
	//
	RCC_APB2ENR |= RCC_ADC1EN; // allumer la clock de ADC1 

	// initialization
	GPIOA_MODER |=  3<<(3*2); // mettre PA3 en mode analogique 
	ADC1_SQR3 = 3; // Configure le canal 3
	ADC1_CR2 |= ADC_ADON; // Allumer le ADC
	
 ////
 /* 3. PD12 = sortie (LED verte) */
    GPIOD_MODER  |= (1 << (12*2)); // j utilise PD12 pour pouvoir s'assurer que le code marche, et que si il y a une erreur c'est sur le bronchement .

	// main loop
	printf("Endless loop!\n");
	
	while(1) {  
		ADC1_CR2 |= ADC_SWSTART; // → démarre une conversion
	while(!(ADC1_SR & ADC_EOC)); // → attend la fin 
		uint16_t val = ADC1_DR; // on lit la valeur
		printf("Luminosity : %d\n", val);
		uint16_t treshold = 2000;
		/// test si luinosity inferieure a 2000 
		if (val < treshold) {
            GPIOD_ODR |=  (1 << 12);   // LED ON
            printf("→ TROP SOMBRE → LED VERTE ON\n");
        } else {
            GPIOD_ODR &= ~(1 << 12);   // LED OFF
            printf("→ OK → LED VERTE OFF\n");
        }
	    /// boucle pour attente passive
		for(volatile int i = 0; i < 300000; i++);
	}

}


