/* Embedded Systems - Exercise 4 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

 // #define NOP() __asm__ volatile ("nop")


static inline void tim4_set_period_ms(unsigned ms) {
    TIM4_PSC = 42000 ;   // tick = 1 ms
    TIM4_ARR = ms ;      // duree de la periode
    TIM4_EGR = 1;           // recharger registres
    TIM4_SR  = 0;           // clear flags
}


int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	// Initialisation du bouton PA0, en entree + pull-down
	GPIOA_MODER &= ~(3 << (USER_BUT*2));
    GPIOA_PUPDR &= ~(3 << (USER_BUT*2));
    GPIOA_PUPDR |=  (2 << (USER_BUT*2));

	// Initialisation de la LED verte PD12 : sortie + push-pull
	 GPIOD_MODER &= ~(3 << (GREEN_LED*2));
    GPIOD_MODER |=  (1 << (GREEN_LED*2));
    GPIOD_OTYPER &= ~(1 << GREEN_LED);
    GPIOD_PUPDR  &= ~(3 << (GREEN_LED*2));

	// TIMER INIT,demarre a 1000ms  (1s)
	  unsigned period = 1000;
    tim4_set_period_ms(period);
    TIM4_CR1 = 1;   // start

    int pressed = 0;  // etat du bouton 
	
	while(1) {
		
		if (TIM4_SR & (1 << 0)) {
            TIM4_SR = 0;
            if (GPIOD_ODR & (1 << GREEN_LED))
                GPIOD_BSRR = 1 << (GREEN_LED + 16);  // OFF
            else
                GPIOD_BSRR = 1 << GREEN_LED;         // ON
        }

        // detection de clic
        if (!pressed && (GPIOA_IDR & (1 << USER_BUT))) {
            // Gestio nde rebound
            for (volatile int i = 0; i < 500000; i++) NOP;
            if (GPIOA_IDR & (1 << USER_BUT)) pressed = 1;
        }
        if (pressed && !(GPIOA_IDR & (1 << USER_BUT))) {
            // gestion de rebond
            for (volatile int i = 0; i < 500000; i++) NOP;
            if (!(GPIOA_IDR & (1 << USER_BUT))) {
                pressed = 0;
                // changer le cycle de clignotement
                if (period == 1000) period = 500;
                else if (period == 500) period = 250;
                else period = 1000;
                tim4_set_period_ms(period);
            }
        }

	}

}
