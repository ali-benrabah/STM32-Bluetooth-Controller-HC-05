/* Embedded Systems - Exercise 2 */

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


int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;

	// GPIO init
	//GPIOA BOUTON

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
	while(1) {
         // for(volatile int = 0: i < 3000)
     if(GPIOA_IDR & (1<<0)){
		// Bouton appuye , PA0 = 1 , on allume LED verte
		GPIOD_BSRR = 1 << GREEN_LED;
	 }else{
		// Bouton relachee , PA0= 0 , on eteint LED verte
		
		GPIOD_BSRR = 1 << (GREEN_LED+16);
	 }


	}

}
