#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/io.h>

#define GREEN_LED   12 
#define ORANGE_LED  13  
// Registres USART1
#define USART1_BASE     0x40011000
#define USART1_SR       _IOREG(USART1_BASE, 0x00)
#define USART1_DR       _IOREG(USART1_BASE, 0x04)
#define USART1_BRR      _IOREG(USART1_BASE, 0x08)
#define USART1_CR1      _IOREG(USART1_BASE, 0x0C)

#define USART_TXE       (1 << 7)
#define USART_RXNE      (1 << 5)
#define USART_UE        (1 << 13)
#define USART_TE        (1 << 3)
#define USART_RE        (1 << 2)

void init_system(void) {
    // Activer les horloges (GPIOA, GPIOD, USART1)
    // Bit 0 = GPIOA (PA9/PA10), Bit 3 = GPIOD (LED)
    RCC_AHB1ENR |= (1 << 0) | (1 << 3); 
    RCC_APB2ENR |= (1 << 4); // USART1

    //  Configurer les LEDs 
    GPIOD_MODER &= ~((0x3 << (GREEN_LED * 2)) | (0x3 << (ORANGE_LED * 2))); 
    GPIOD_MODER |=  ((0x1 << (GREEN_LED * 2)) | (0x1 << (ORANGE_LED * 2)));

    //  configurer bluetooth PA9/PA10 en alternate function (AF7)
    // PA9 = TX, PA10 = RX ( ou branchement contraire)
    // Bits 18-19 pour PA9, Bits 20-21 pour PA10
    GPIOA_MODER &= ~((0x3 << 18) | (0x3 << 20)); // Reset
    GPIOA_MODER |=  ((0x2 << 18) | (0x2 << 20)); // Mode AF (10)

    // AF7 pour PA9 et PA10
    // PA9 est sur les bits 4-7 de AFRH
    // PA10 est sur les bits 8-11 de AFRH
    GPIOA_AFRH  &= ~((0xF << 4) | (0xF << 8)); // Reset
    GPIOA_AFRH  |=  ((0x7 << 4) | (0x7 << 8)); // Set AF7 (0111)

    // 9600 Bauds : 84000000 / 9600 = 8750 = 0x222E
    USART1_BRR = 0x222E; 

    // Activer UART
    USART1_CR1 = USART_UE | USART_TE | USART_RE;
}

void send_char(char c) {
    while (!(USART1_SR & USART_TXE));
    USART1_DR = c;
}

int main(void) {
    init_system();
    
    volatile int compteur_temps = 0;
    int etat_led_orange = 0;

    // Message de test au demarrage
    send_char('O'); send_char('K'); send_char('\n');

    while(1) {
        // led orange qui clignotte en permanence (pour tester le code)
        compteur_temps++;
        if (compteur_temps > 400000) { 
            if (etat_led_orange == 0) {
                GPIOD_BSRR = (1 << ORANGE_LED); 
                etat_led_orange = 1;
            } else {
                GPIOD_BSRR = (1 << (ORANGE_LED + 16)); 
                etat_led_orange = 0;
            }
            compteur_temps = 0;
        }

        // reception bluetooth
        if (USART1_SR & USART_RXNE) {
            char recu = USART1_DR; // lire le caractere recu 

            if (recu == '1') {
                GPIOD_BSRR = (1 << GREEN_LED); // allume Verte
            }
            else if (recu == '0') {
                GPIOD_BSRR = (1 << (GREEN_LED + 16)); // eteint verte
            }
        }
    }
    return 0;
}