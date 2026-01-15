/* Embedded Systems - Exercise 1 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>



// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15


// GPIOA
#define USER_BUT	0





int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIODEN;

	// GPIO init
      
       // On met des 0 dans les bit de 24 a 31 pour nettoyer
     GPIOD_MODER &= ~(1 << GREEN_LED*2);
	 GPIOD_MODER &= ~(1 << ORANGE_LED*2);
	 GPIOD_MODER &= ~(1 << RED_LED*2);
	 GPIOD_MODER &= ~(1 << BLUE_LED*2);
     
      // on met les bit de 24 a 31 en 01010101
	 GPIOD_MODER |= (0x55 << 24);

     // On configure OTYPER en Output mode -> 1 (push-pull)

     GPIOD_OTYPER &= ~(0xF << 12); 

     // on configuer PUPDR

     GPIOD_PUPDR &= ~(0xFF <<24);

	printf("Endless loop!\n");
    // on utilise un tableau qui a tout les etat de led possibles;
    const int order[4] = { GREEN_LED, ORANGE_LED, RED_LED, BLUE_LED };
    int k=0;

	while(1) {
         int num_led = order[k];


        GPIOD_BSRR = (1u << (GREEN_LED +16))|
                    (1u << (ORANGE_LED +16))|
                    (1u << (RED_LED +16))|
                    (1u << (BLUE_LED +16));

        //Allumer la led courante 
        GPIOD_BSRR = (1u << num_led);

        // LA BOUCLE D'ATTENTE
        for(volatile int i=0; i<30000000; i++) NOP;
        
        // on avance vers la led suivante dans le tableau
        if ( k ==3){
            k=0;
        } else { k = k+1;}

        


			 
		
    }

	

}
