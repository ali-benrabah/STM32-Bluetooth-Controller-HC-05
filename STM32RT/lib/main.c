/**
 * Torpy - grid-follower wheeling and box delivering bot.
 * Copyright (C) 2021  Université de Toulouse <casse@irit.fr>
 */

#include <tinyprintf.h>
#include <stm32f4/led.h>
#include <stm32f4/rcc.h>
#include <stm32f4/tim.h>
#include <stm32f4/gpio.h>

volatile int n = 10;

////////////////////// ADDED FOR HC-05
#define BT_BAUD 9600       
#define MAX_STRLEN 20

#define USART1_BASE 0x40011000
#define USART1_SR   (*((volatile uint32_t *)(USART1_BASE + 0x00)))
#define USART1_DR   (*((volatile uint32_t *)(USART1_BASE + 0x04)))
#define USART1_BRR  (*((volatile uint32_t *)(USART1_BASE + 0x08)))
#define USART1_CR1  (*((volatile uint32_t *)(USART1_BASE + 0x0C)))
#define NVIC_ISER1  (*((volatile uint32_t *)0xE000E104))
/////////////////////

#define PERIOD  30000000
#define WAIT(n) \
    for (int i = 0; i < (n); i++) \
        __asm__("nop");

// GPIOD
#define GREEN_LED   12
#define ORANGE_LED  13
#define RED_LED     14
#define BLUE_LED    15

// GPIODA
#define USER_BUT    0

/* --- FONCTIONS UTILITAIRES POUR LE BLUETOOTH --- */

void usart1_send_char(char c) {
    // Attendre que le registre de transmission soit vide (TXE)
    while (!(USART1_SR & (1 << 7))); 
    USART1_DR = c;
}

void usart1_send_str(char *str) {
    while (*str) {
        usart1_send_char(*str++);
    }
}

void configure_hc05() {
    // 1. Passer temporairement à 38400 bauds (Vitesse du mode AT)
    // Calcul pour 84 MHz : 84000000 / 38400 = 2187.5 -> Mantisse 0x88B, Frac 0x8
    USART1_BRR = 0x88B8; 

    // Pause pour stabilisation
    for(volatile int i=0; i<200000; i++);

    // 2. Envoyer les commandes AT
    // Test
    usart1_send_str("AT\r\n");
    for(volatile int i=0; i<5000000; i++); 

    // Mode Esclave
    usart1_send_str("AT+ROLE=0\r\n");
    for(volatile int i=0; i<5000000; i++);

    // Nom du robot
    usart1_send_str("AT+NAME=TorpyBot\r\n");
    for(volatile int i=0; i<5000000; i++);

    // Fixer la vitesse normale à 9600, 1 stop bit, pas de parité
    usart1_send_str("AT+UART=9600,0,0\r\n");
    for(volatile int i=0; i<5000000; i++);

    // 3. IMPORTANT : Remettre le STM32 à 9600 bauds pour la suite !
    USART1_BRR = 0x222E; 
}

/* --- FIN FONCTIONS UTILITAIRES --- */

void init_bluetooth() {
    // 1. Activer les horloges (GPIOB et USART1)
    RCC_AHB1ENR |= (1 << 1); 
    RCC_APB2ENR |= (1 << 4); 

    // 2. Configurer PB6 et PB7 en mode Alternate Function (AF)
    GPIOB_MODER &= ~((3 << 12) | (3 << 14)); 
    GPIOB_MODER |=  ((2 << 12) | (2 << 14)); 

    // 3. Assigner l'AF7 (USART1) aux pins PB6 et PB7
    GPIOB_AFRL &= ~((0xF << 24) | (0xF << 28)); 
    GPIOB_AFRL |=  ((7 << 24) | (7 << 28));     

    // 4. Vitesse : 9600 bauds pour APB2 @ 84MHz
    // 84000000 / 9600 = 8750 = 0x222E
    USART1_BRR = 0x222E; 

    // 5. Activer UART + Interruption RX
    USART1_CR1 = (1 << 13) | (1 << 5) | (1 << 3) | (1 << 2);

    // 6. Activer l'interruption NVIC
    NVIC_ISER1 |= (1 << 5); 
}

void USART1_IRQHandler(void) {
    if (USART1_SR & (1 << 5)) {
        char c = USART1_DR; 

        if (c == '1') {
            GPIOD_BSRR = 1 << ORANGE_LED; // Allume Orange
        }
        else if (c == '0') {
            GPIOD_BSRR = 1 << (ORANGE_LED + 16); // Éteint Orange
        }
    }
}

int main() {
    printf("Starting...\n");

    RCC_AHB1ENR |= RCC_GPIODEN;

    // GPIO init
    GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*GREEN_LED, 2, GPIO_MODER_OUT);
    GPIOD_BSRR = 1 << (GREEN_LED + 16);
    GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*ORANGE_LED, 2, GPIO_MODER_OUT);
    GPIOD_BSRR = 1 << (ORANGE_LED + 16);
    GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*RED_LED, 2, GPIO_MODER_OUT);
    GPIOD_BSRR = 1 << (RED_LED + 16);
    GPIOD_MODER = SET_BITS(GPIOD_MODER, 2*BLUE_LED, 2, GPIO_MODER_OUT);
    GPIOD_BSRR = 1 << (BLUE_LED + 16);

    printf("LED switched on!\n");

    init_bluetooth();
    
    // --- ZONE DE CONFIGURATION ---
    // 1. Branche KEY du HC-05 au 3.3V
    // 2. Lance le programme
    // 3. Une fois fait, tu peux commenter la ligne ci-dessous :
    configure_hc05(); 
    // -----------------------------
    
    while(1) {
        printf("Running...\n");

        /* Clignotement de vie (Vert) */
        GPIOD_BSRR = 1 << GREEN_LED;
        WAIT(PERIOD);
        GPIOD_BSRR = 1 << (GREEN_LED + 16);
        WAIT(PERIOD);
    }
}