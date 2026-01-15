/* Embedded Systems - Exercise 7 */
// avec gestion de rebound 
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>

#define GREEN_LED 12
#define USER_BUT  0

volatile uint8_t debounce_flag = 0;

// --- EXTI0 Handler ---
void handle_EXTI0(void)
{
    if (EXTI_PR & (1 << 0)) {
        EXTI_PR = (1 << 0);
        
        // Démarrer un timer de 20ms pour débouncer
        TIM4_CNT = 0;
        TIM4_CR1 |= TIM_CEN;
        debounce_flag = 1;
    }
}

// --- TIM4 Handler (debounce) ---
void handle_TIM4(void)
{
    if (TIM4_SR & TIM_UIF) {
        TIM4_SR = 0;
        TIM4_CR1 &= ~TIM_CEN;  // Stop timer
        
        // Vérifier que le bouton est TOUJOURS appuyé
        if (GPIOA_IDR & (1 << 0)) {
            GPIOD_ODR ^= (1 << GREEN_LED);
            printf("LED toggled!\n");
        }
        
        debounce_flag = 0;
    }
}

int main() {
    printf("\nEX7 - Debounce with Timer\n");

    // --- Clocks ---
    RCC_AHB1ENR |= RCC_GPIOAEN | RCC_GPIODEN;
    RCC_APB2ENR |= RCC_SYSCFGEN;
    RCC_APB1ENR |= RCC_TIM4EN;

    // --- GPIO ---
    GPIOA_MODER &= ~(0x3 << 0);  // PA0 input
    GPIOD_MODER = (GPIOD_MODER & ~(0x3 << (12*2))) | (1 << (12*2));
    GPIOD_OTYPER &= ~(1 << 12);
    GPIOD_PUPDR &= ~(0x3 << (12*2));

    // --- EXTI0 ---
    SYSCFG_EXTICR1 &= ~(0xF << 0);
    EXTI_IMR |= (1 << 0);
    EXTI_RTSR |= (1 << 0);
    EXTI_FTSR &= ~(1 << 0);

    // --- TIM4 (20ms @ 10kHz) ---
    TIM4_PSC = 4199;     // 42MHz / 4200 = 10kHz
    TIM4_ARR = 200;      // 200 × 100µs = 20ms
    TIM4_DIER |= TIM_UIE;
    TIM4_CR1 |= TIM_ARPE;

    // --- NVIC ---
    DISABLE_IRQS;
    NVIC_IRQ(6)  = (uint32_t)handle_EXTI0;   // EXTI0
    NVIC_IRQ(30) = (uint32_t)handle_TIM4;    // TIM4
    NVIC_ISER(0) |= (1 << 6);                // EXTI0
    NVIC_ISER(0) |= (1 << 30);               // TIM4
    ENABLE_IRQS;

    printf("Press button - debounced toggle!\n");

    while (1) {
       
    }
}
