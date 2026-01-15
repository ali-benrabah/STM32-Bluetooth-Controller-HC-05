/* Embedded Systems - Exercise 3 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>



// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

////



int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	// GPIO init

	GPIOA_MODER &= ~(0x3 << USER_BUT*2);
	GPIOA_PUPDR &= ~(0b11 << USER_BUT*2);
    GPIOA_PUPDR |=  (0b10 << USER_BUT*2);


	//GPIOD LED
	// initialiser MODER12
	GPIOD_MODER &= ~(3 << (GREEN_LED*2));
	
	/////////// MODER12 = 01 (sortie)
	GPIOD_MODER |= (1 << (GREEN_LED*2));

	//////////////////////// OTYPER12 = 0 (push-pull)
	GPIOD_OTYPER &= ~(1 << GREEN_LED);

	////////////////// PUPDR12 = 00
	GPIOD_PUPDR &= ~(3 << (GREEN_LED*2));


	printf("Endless loop!\n");

int but_state = 0;   // 0 = relachee, 1 = appuyee
int led_on = 0;   // 0 = OFF, 1 = ON


	while(1) {

		if (GPIOA_IDR & (1 << USER_BUT)) {
        // Bouton pressed
        but_state = 1;
    }
    else if (but_state == 1) {
        // Transition appui -> relachement => clic detecteee
        but_state = 0;
			for (volatile int i = 0; i < 500000; i++) NOP; // Un delai pour gerer les rebonds

        led_on = !led_on;   // Inverser etat preced

        if (led_on) {
            GPIOD_BSRR = 1 << GREEN_LED;          // Allumer
        } else {
            GPIOD_BSRR = 1 << (GREEN_LED + 16);   // Eteindre
        }
    }

	}

}
