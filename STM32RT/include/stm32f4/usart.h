#ifndef STM32F4_USART_H
#define STM32F4_USART_H

#include "io.h"

// Adresse de base pour USART1 (APB2)
#define USART1_BASE    0x40011000
#define USART1_REG(o)  _IOREG(USART1_BASE, o)

// Registres
#define USART1_SR      USART1_REG(0x00)
#define USART1_DR      USART1_REG(0x04)
#define USART1_BRR     USART1_REG(0x08)
#define USART1_CR1     USART1_REG(0x0C)
#define USART1_CR2     USART1_REG(0x10)
#define USART1_CR3     USART1_REG(0x14)

// Bits du registre Status (SR)
#define USART_RXNE     (1 << 5)  // Read data register not empty
#define USART_TXE      (1 << 7)  // Transmit data register empty

// Bits du registre Control 1 (CR1)
#define USART_RE       (1 << 2)  // Receiver Enable
#define USART_TE       (1 << 3)  // Transmitter Enable
#define USART_UE       (1 << 13) // USART Enable

#endif