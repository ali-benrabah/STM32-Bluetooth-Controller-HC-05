/* Embedded Systems - Exercise 15 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>

// Définition de la Pin (PA6)
#define SERVO_PIN   6 

// Bornes du servomoteur (en microsecondes)
#define PULSE_MIN   1000  // 0 degrés 
#define PULSE_MAX   2000  // 180 degrés

// Fonction d'attente 
void wait_ms(int ms) {
    for (volatile int i = 0; i < ms * 4000; i++) {
        __asm__("nop");
    }
}



int main() {
    printf("\nStarting Servo on PA6...\n");

    // 1. Activer les horloges : GPIOA (Bit 0) et TIM3 (Bit 1 sur APB1)
    RCC_AHB1ENR |= (1 << 0); 
    RCC_APB1ENR |= (1 << 1); 

    // 2. Configurer PA6 en mode Alternate Function
    // MODER : Mettre '10' (Alternate Function) à la position 2*6 = 12
    GPIOA_MODER &= ~(0x3 << (2 * SERVO_PIN)); // Reset
    GPIOA_MODER |=  (0x2 << (2 * SERVO_PIN)); // Set AF

    // 3. Assigner l'AF2 (TIM3) à PA6
    // AFRL : PA6 est sur les bits 24-27. On veut AF2 (0010)
    GPIOA_AFRL &= ~(0xF << 24); // Reset
    GPIOA_AFRL |=  (0x2 << 24); // Set AF2

    // 4. Configuration du Timer 3 pour le PWM
    // Horloge Timer = 84 MHz. On veut 1 tick = 1 µs.
    // PSC = 84 - 1 = 83
    TIM3_PSC = 83; 
    
    // Période de 20 ms = 20000 µs 
    TIM3_ARR = 19999; 
    
    // 5. Configurer le canal 1 (CH1) en mode PWM 1
    // CCMR1 : OC1M = 110 (Bits 6:4) et OC1PE = 1 (Preload Enable - Bit 3)
    TIM3_CCMR1 &= ~((0x7 << 4) | (1 << 3)); // Reset
    TIM3_CCMR1 |=  ((0x6 << 4) | (1 << 3)); // PWM mode 1 + Preload

    // 6. Activer la sortie (Enable Output)
    TIM3_CCER |= 1; // CC1E
    
    // Initialiser à 0°
    TIM3_CCR1 = PULSE_MIN;
    
    // Lancer le timer
    TIM3_CR1 |= 1;

    printf("Servo initialized. Starting wave...\n");

    int current_pulse = PULSE_MIN;
    int step = 10;
    int direction = 1;

    // Boucle principale : Animation Flag Waver    
	while(1) {
        // Mettre à jour la largeur d'impulsion
        TIM3_CCR1 = current_pulse;

        // Calculer la prochaine position
        current_pulse += (step * direction);

        // Inverser la direction si on touche les bornes
        if (current_pulse >= PULSE_MAX) {
            current_pulse = PULSE_MAX;
            direction = -1; // Descente
        }
        else if (current_pulse <= PULSE_MIN) {
            current_pulse = PULSE_MIN;
            direction = 1;  // Montée
        }

        // Délai pour que le mouvement soit visible (~2 secondes par vague)
        wait_ms(10);
    }
}
