/* Embedded Systems - Exercise 6 */


#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>
#include <stm32f4/nvic.h>

// GPIOD
#define GREEN_LED    12
#define ORANGE_LED    13
#define RED_LED        14
#define BLUE_LED    15

// GPIODA
#define USER_BUT    0



void handle_TIM4(void)
{
    if ((TIM4_SR & TIM_UIF) !=  0) {
        GPIOD_ODR ^= (1 << GREEN_LED);  // <— THIS LINE BLINKS THE LED
    }
    TIM4_SR = 0;
}

void init_gpio(){
    GPIOD_MODER = (GPIOD_MODER & ~(0x3 << (12*2))) | (1 << (12*2));
    GPIOD_OTYPER &= ~(1 << GREEN_LED);
    // No pull-up/down
    GPIOD_PUPDR &= ~(0x3 << (12*2));
}

int main() {
    printf("\nStarting...\n");

    // RCC init
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_APB1ENR |= RCC_TIM4EN;

    init_gpio();
    /* 3. Configure TIM4: 500 ms period */
    TIM4_PSC = 4199;      // 42MHz / 4200 = 10kHz (100us tick)
    TIM4_ARR = 5000;      // 5000 ticks × 100us = 500ms

    TIM4_DIER |= TIM_UIE; // Enable update interrupt
    TIM4_CR1  |= TIM_ARPE;// Auto-reload preload

    DISABLE_IRQS;
    NVIC_ISER(0) |= (1 << 30);  // IRQ 30
    NVIC_ICPR(TIM4_IRQ >> 5) = (1 << (TIM4_IRQ & 0x1F));
    NVIC_IRQ(TIM4_IRQ) = (uint32_t)handle_TIM4;
    NVIC_IPR(TIM4_IRQ) = 0;
    NVIC_ISER(TIM4_IRQ >> 5) = (1 << (TIM4_IRQ & 0x1F));
    ENABLE_IRQS;


    TIM4_CR1  |= TIM_CEN; // Start timer


    // main loop
    printf("Endless loop!\n");
    while(1) {

    }

}

