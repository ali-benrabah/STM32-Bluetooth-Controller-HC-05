/* Embedded Systems - Exercise 9 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>

#define LED_COUNT 6
const uint8_t LED_PINS[LED_COUNT] = {10, 11, 12, 13, 14, 15};

volatile uint8_t current_step = 0;
volatile uint8_t direction = 1;  // 1 = allumage, 0 = extinction

// GPIOD
#define led_1 10
#define led_2 11
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

void init_leds() {
    RCC_AHB1ENR |= RCC_GPIODEN;  // Clock GPIOD

    for (int i = 0; i < LED_COUNT; i++) {
        uint8_t pin = LED_PINS[i];
        GPIOD_MODER |= (1 << (pin * 2));        // Output mode
        GPIOD_OTYPER &= ~(1 << pin);            // Push-pull
        GPIOD_PUPDR &= ~(0x3 << (pin * 2));     // No pull-up/down
    }
}

void handle_tim4(void) {
    if (TIM4_SR & TIM_UIF) {
        TIM4_SR = 0;  // Clear interrupt flag

        if (direction) {
            // Allumer la LED suivante // ON REMONTE PD10 -> PD15 ( on utilisant current_step++)
            GPIOD_ODR |= (1 << LED_PINS[current_step]); 
            if (++current_step >= LED_COUNT) { // verifier si current_step + 1 >= 6 
               // on eteint la LED et on change de direction  ( 0 pour descendre ) 
				current_step = LED_COUNT - 1;
                direction = 0;
            }
        } else {
            // Éteindre la LED courante et on recule 
            GPIOD_ODR &= ~(1 << LED_PINS[current_step]);
            if (current_step-- == 0) {
                current_step = 0;
                direction = 1;
            }
        }
    }
}

int main() {
	printf("\nStarting...\n");
	init_leds();
	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;



	// initialization
 
	TIM4_PSC = 8399;     // 84 MHz / 8400 = 10 kHz
    TIM4_ARR = 5000;     // 5000 × 100µs = 500ms
    TIM4_DIER |= TIM_UIE; // Enable update interrupt
    TIM4_CR1 |= TIM_ARPE; // Auto-reload preload

    // NVIC
    DISABLE_IRQS;
    NVIC_IRQ(30) = (uint32_t)handle_tim4;
    NVIC_ISER(0) |= (1 << 30);  // IRQ 30 = TIM4
    ENABLE_IRQS;

    TIM4_CR1 |= TIM_CEN;  // Start timer
	// main loop
	printf("Endless loop!\n");
	while(1) {
	}

}


