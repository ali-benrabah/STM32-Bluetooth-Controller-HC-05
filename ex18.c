#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>
#include <stm32f4/io.h>

// --- ADRESSES USART2 (PA2/PA3) ---
#define USART2_BASE     0x40004400
#define USART2_SR       _IOREG(USART2_BASE, 0x00)
#define USART2_DR       _IOREG(USART2_BASE, 0x04)
#define USART2_BRR      _IOREG(USART2_BASE, 0x08)
#define USART2_CR1      _IOREG(USART2_BASE, 0x0C)

#define USART_TXE       (1 << 7)
#define USART_UE        (1 << 13)
#define USART_TE        (1 << 3)

// --- PINOUT ---
#define TRIG_PIN    9       // PA9 (Capteur)
#define ECHO_PIN    8       // PA8 (Capteur)
#define LED_PWM     6       // PA6
#define BUZZER      5       // PA5

#define DISTANCE_MAX_LED    600
#define DISTANCE_ALERTE     300

int last_state = -1;

void init_hardware() {
    // 1. Horloges
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_APB1ENR |= RCC_TIM2EN | RCC_TIM3EN | (1 << 17); // TIM2, TIM3, USART2

    // 2. Nettoyage TOTAL des broches utilisées (Sécurité)
    // On efface les modes précédents pour PA2,3,5,6,8,9
    GPIOA_MODER &= ~( (3<<(2*2)) | (3<<(3*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(8*2)) | (3<<(9*2)) );

    // 3. Config CAPTEUR & BUZZER (Sorties)
    GPIOA_MODER |= (1 << (TRIG_PIN * 2)) | (1 << (BUZZER * 2));
    
    // 4. Config LED (AF2)
    GPIOA_MODER |= (2 << (LED_PWM * 2));
    GPIOA_AFRL  &= ~(0xF << (LED_PWM * 4)); // Clean AF
    GPIOA_AFRL  |= (2 << (LED_PWM * 4));    // Set AF2

    // 5. Config BLUETOOTH (AF7 sur PA2/PA3)
    GPIOA_MODER |= (2 << (2*2)) | (2 << (3*2)); // Mode AF
    GPIOA_AFRL  &= ~((0xF << 8) | (0xF << 12)); // Clean AF pour PA2/PA3
    GPIOA_AFRL  |= (7 << 8) | (7 << 12);        // Set AF7 (USART2)

    // 6. Config USART2
    // ESSAI 1 (Standard) : 4375  (Si signes bizarres -> Essaie 8750)
    USART2_BRR = 2187; 
    USART2_CR1 = USART_UE | USART_TE; 

    // 7. Timers (Config Ex17)
    TIM2_PSC = 84 - 1; TIM2_ARR = 0xFFFFFFFF; TIM2_CR1 |= TIM_CEN;
    TIM3_PSC = 84 - 1; TIM3_ARR = 1000;
    TIM3_CCMR1 |= (6 << 4); TIM3_CCER |= TIM_CC1E; TIM3_CR1 |= TIM_CEN;
}

// Envoi Bluetooth
void uart_send(char* s) {
    while(*s) {
        while(!(USART2_SR & USART_TXE));
        USART2_DR = *s++;
    }
}

void delay_ms(int ms) {
    TIM2_CNT = 0;
    while (TIM2_CNT < (ms * 1000));
}

int main() {
    init_hardware();
    
    // Test Bluetooth au démarrage
    uart_send("SYSTEM_READY\n");

    while (1) {
        // --- CAPTEUR (Logique EX17 Intacte) ---
        GPIOA_ODR |= (1 << TRIG_PIN);
        for (volatile int i = 0; i < 5000; i++); 
        GPIOA_ODR &= ~(1 << TRIG_PIN);

        long timeout = 100000;
        while (!(GPIOA_IDR & (1 << ECHO_PIN))) if (--timeout == 0) break;

        if (timeout > 0) {
            TIM2_CNT = 0;
            while (GPIOA_IDR & (1 << ECHO_PIN)); 
            uint32_t distance = TIM2_CNT / 58;

            int current_state = 0;

            // Logique LED/Buzzer
            if (distance < DISTANCE_MAX_LED && distance > 0) {
                TIM3_CCR1 = (DISTANCE_MAX_LED - distance) * (1000 / DISTANCE_MAX_LED);
                current_state = 1; 
            } else {
                TIM3_CCR1 = 0; 
                current_state = 0;
            }

            if (distance < DISTANCE_ALERTE && distance > 0) {
                GPIOA_ODR |= (1 << BUZZER); 
                current_state = 2; 
            } else {
                GPIOA_ODR &= ~(1 << BUZZER); 
            }

            // Envoi Bluetooth
            if (current_state != last_state) {
                if (current_state == 2) uart_send("ALERT\r\n");
                else if (current_state == 1) uart_send("DETECT\r\n");
                else uart_send("NORMAL\r\n");
                last_state = current_state;
            }

        } else {
            // Reset trig si erreur
            GPIOA_ODR &= ~(1 << TRIG_PIN);
        }

        delay_ms(60); 
    }
    return 0;
}