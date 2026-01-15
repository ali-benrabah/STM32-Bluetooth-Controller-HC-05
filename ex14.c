/* Embedded Systems - Exercise 14 */


#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>

// GPIOD (LEDs de la carte - non utilisées ici)
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIOA
#define USER_BUT	0

// LEDs externes sur GPIOB
const uint8_t leds[4] = {11, 12, 13, 4};  // PB11 → PB12 → PB13 → PB14

/* LEDs externes sur PB11, PB12, PB13, PB14 */
/* Potentiomètre sur PA2 (ADC1 channel 2) */


int main() { 
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;   // PA2 (potentiomètre)
	RCC_AHB1ENR |= RCC_GPIOBEN;   // PB11-14 (LEDs externes)
	RCC_APB2ENR |= RCC_ADC1EN;    // ADC1

	//  Configuration PA2 en entrée analogique
	GPIOA_MODER |= (3 << (2 * 2));   // PA2 = 11 (analog mode)

	//  Configuration PB11 à PB14 en sortie push-pull 
	for (int i = 0; i < 4; i++) {
		uint8_t pin = leds[i];
		GPIOB_MODER |= (1 << (pin * 2));        // 01  output
		GPIOB_OTYPER &= ~(1 << pin);            // 0  push-pull
		GPIOB_PUPDR &= ~(3 << (pin * 2));       // 00  no pull
	}

	// === Configuration ADC1 - canal 2 (PA2) ===
	ADC1_SQR3 = 2;           // Premier (et seul) canal de la séquence = canal 2
	ADC1_CR2 |= ADC_ADON;    // Allume l'ADC

	// === Boucle principale ===
	printf("Endless loop! Vue-meter actif (PB11-PB14)\n");

	while(1) {
		// 1. Démarre une conversion
		ADC1_CR2 |= ADC_SWSTART;

		// 2. Attend la fin de conversion
		while (!(ADC1_SR & ADC_EOC));

		// 3. Lit la valeur (0 à 4095)
		uint16_t value = ADC1_DR;

		// 4. Convertit en niveau 0 à 4 (arrondi propre)
		uint8_t level = (value + 512) / 1024;   // 0..4095 → 0..4
		if (level > 4) level = 4;

		// 5. Éteint toutes les LEDs
		GPIOB_ODR &= ~((1<<11)|(1<<12)|(1<<13)|(1<<14));

		// 6. Allume les 'level' premières LEDs
		for (uint8_t i = 0; i < level; i++) {
			GPIOB_ODR |= (1 << leds[i]);
		}

		// 7. Affichage sur le terminal
		printf("Potentiometre = %4u -> %d LED(s) allumee(s)\n", value, level);

	
		for (volatile int i = 0; i < 100000; i++); 
	}
		
	
    
}



