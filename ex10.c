/* Embedded Systems - Exercise 10*/


#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/nvic.h>
#include <stm32f4/tim.h>

#define BUTTON_PIN  0   // PA0 Bouton bleu intégré
#define LED_PIN    12   // PD12 LED verte intégrée
 // j utilise 0 pour verifier ave cle bouton bleu sur la carte , si ca marche avec le bouton bleu et pas avec le bouton externe 
 // ca veut dire que on code est correcte mais le bronchement est mal foutu 
volatile uint8_t debounce_pending = 0;

/* Handler EXTI0 (IRQ 6) - PA0 */
void handle_EXTI0(void)
{
    if (EXTI_PR & (1 << BUTTON_PIN)) {
        EXTI_PR = (1 << BUTTON_PIN);  // Acquittement

        if (!debounce_pending) {
            TIM4_CNT = 0;
            TIM4_CR1 |= TIM_CEN;       // Démarre timer 20ms
            debounce_pending = 1;
        }
    }
}

/* Handler TIM4 - Debounce 20ms */
void handle_TIM4(void)
{
    if (TIM4_SR & TIM_UIF) {
        TIM4_SR = 0;
        TIM4_CR1 &= ~TIM_CEN;          // Stop timer
        debounce_pending = 0;

        // Vérifie que le bouton est TOUJOURS appuyé
        if (GPIOA_IDR & (1 << BUTTON_PIN)) {
            GPIOD_ODR ^= (1 << LED_PIN);  // Toggle LED verte
			// afficher sur make log 
            printf("Bouton bleu (PA0) appuye → LED verte (PD12) toggle!\n");
        }
    }
}

int main() {
	//voir sur make log 
    printf("\n=== EX10 - Bouton bleu PA0 → LED verte PD12 ===\n");

    /* 1. Clocks */
    RCC_AHB1ENR |= RCC_GPIOAEN;     // GPIOA (PA0)
    RCC_AHB1ENR |= RCC_GPIODEN;     // GPIOD (PD12)
    RCC_APB2ENR |= RCC_SYSCFGEN;    // SYSCFG pour EXTI
    RCC_APB1ENR |= RCC_TIM4EN;      // TIM4 pour debounce

    /* 2. GPIOs */
    // PA0 = entrée (00) - déjà par défaut
    GPIOA_MODER &= ~(0x3 << (BUTTON_PIN * 2));

    // PD12 = sortie (01)
    GPIOD_MODER = (GPIOD_MODER & ~(0x3 << (LED_PIN * 2))) | (1 << (LED_PIN * 2));
    GPIOD_OTYPER &= ~(1 << LED_PIN);
    GPIOD_PUPDR &= ~(0x3 << (LED_PIN * 2));

    /* 3. EXTI0 → PA0 */
    SYSCFG_EXTICR1 &= ~(0xF << 0);  // EXTICR1[3:0] = 0 → Port A // par << 0 on cible les 4 premiers bits
    EXTI_IMR |= (1 << BUTTON_PIN);
    EXTI_RTSR |= (1 << BUTTON_PIN);  // Front montant
    EXTI_FTSR &= ~(1 << BUTTON_PIN); // on desactive le front descendant

    /* 4. TIM4 - Debounce 20ms */
    TIM4_PSC = 8399;     // 84MHz / 8400 = 10kHz
    TIM4_ARR = 200;      // 200 × 100µs = 20ms
    TIM4_DIER |= TIM_UIE;

    /* 5. NVIC */
    DISABLE_IRQS;
    NVIC_IRQ(6)  = (uint32_t)handle_EXTI0;  // EXTI0_IRQn = 6
    NVIC_IRQ(30) = (uint32_t)handle_TIM4;   // TIM4_IRQn = 30
    NVIC_ISER(0) |= (1 << 6);
    NVIC_ISER(0) |= (1 << 30);
    ENABLE_IRQS;

    printf("Appuie sur le bouton bleu de la carte...\n");

    while (1) {
        // Tout est géré par interruption
    }
}

