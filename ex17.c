#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>

// --- Configuration des Pins ---
#define TRIG_PIN    9       // PA9  -> Trig
#define ECHO_PIN    8       // PA8  -> Echo
#define LED_PWM     6       // PA6  -> LED (TIM3_CH1)
#define BUZZER      5       // PA5  -> Buzzer

// --- Paramètres ---
#define DISTANCE_MAX_LED    600  // La LED commence à s'allumer à 50cm
#define DISTANCE_ALERTE     300   // Le Buzzer sonne à 15cm

void init_hardware() {
    // 1. Horloges
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_APB1ENR |= RCC_TIM2EN | RCC_TIM3EN; // TIM2 (Mesure), TIM3 (LED)

    // 2. GPIO
    // PA9 (Trig) & PA5 (Buzzer) en Sortie
    GPIOA_MODER |= (1 << (TRIG_PIN * 2)) | (1 << (BUZZER * 2));
    
    // PA8 (Echo) en Entrée
    GPIOA_MODER &= ~(3 << (ECHO_PIN * 2));

    // PA6 (LED) en mode Alternate Function (AF) pour le PWM
    GPIOA_MODER |= (2 << (LED_PWM * 2));
    GPIOA_AFRL  |= (2 << (LED_PWM * 4)); // AF2 = TIM3

    // 3. TIM2 : Chronomètre (1 MHz -> 1 µs par tic)
    TIM2_PSC = 84 - 1;     // Réglage pour 84MHz (essaie 16-1 si horloge de base)
    TIM2_ARR = 0xFFFFFFFF; // Max
    TIM2_CR1 |= TIM_CEN;

    // 4. TIM3 : PWM pour la LED (1 kHz)
    // On veut une valeur de 0 à 1000 pour la luminosité
    TIM3_PSC = 84 - 1;
    TIM3_ARR = 1000;       // Période de 1000 coups
    TIM3_CCMR1 |= (6 << 4); // Mode PWM 1
    TIM3_CCER  |= TIM_CC1E; // Active la sortie
    TIM3_CR1   |= TIM_CEN;  // Démarre le timer
}

// Petite fonction pour faire des pauses sans bloquer tout le système
void delay_ms(int ms) {
    TIM2_CNT = 0;
    while (TIM2_CNT < (ms * 1000));
}

int main() {
    init_hardware();
    printf("\n=== Alarme Protection Boite ACTIVE ===\n");

    while (1) {
        // --- 1. Envoi du TRIG (Version Longue) ---
        GPIOA_ODR |= (1 << TRIG_PIN);
        // Ta correction : impulsion longue pour réveiller le capteur
        for (volatile int i = 0; i < 5000; i++); 
        GPIOA_ODR &= ~(1 << TRIG_PIN);

        // --- 2. Lecture de l'ECHO (Mode Bloquant sur) ---
        // Attente du début du signal (Montée)
        long timeout = 100000;
        while (!(GPIOA_IDR & (1 << ECHO_PIN))) {
            if (--timeout == 0) break;
        }

        if (timeout > 0) {
            // Mesure de la durée
            TIM2_CNT = 0;
            while (GPIOA_IDR & (1 << ECHO_PIN)); // Attente descente
            uint32_t duration = TIM2_CNT;
            
            // Calcul distance
            uint32_t distance = duration / 58;

            // --- 3. Logique LED (PWM) ---
            // Plus on est près, plus ça brille
            uint32_t duty = 0;

            if (distance < DISTANCE_MAX_LED && distance > 0) {
                // Règle de trois inversée :
                // Dist 0cm  -> Duty 1000 (Max)
                // Dist 50cm -> Duty 0    (Eteint)
                duty = (DISTANCE_MAX_LED - distance) * (1000 / DISTANCE_MAX_LED);
            } else {
                duty = 0; // Trop loin
            }
            TIM3_CCR1 = duty; // Applique la luminosité

            // --- 4. Logique BUZZER ---
            if (distance < DISTANCE_ALERTE && distance > 0) {
                GPIOA_ODR |= (1 << BUZZER); // SONNE !
                printf("!!! ALERTE !!! Intrus a %lu cm\n", distance);
            } else {
                GPIOA_ODR &= ~(1 << BUZZER); // Silence
                printf("Surveillance... Dist: %lu cm\n", distance);
            }

        } else {
            // Si le capteur rate une mesure, on ne fait rien et on réessaie
            printf("Erreur lecture capteur (Timeout)\n");
        }

        // Petite pause pour stabiliser les ultrasons (éviter les échos fantômes)
        delay_ms(60); 
    }
    return 0;
}