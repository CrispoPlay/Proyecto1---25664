/*
Universidad del Valle de Guatemala
Proyecto 1 - Programación de Microprocesadores
Autor: Cristian Estuardo Orellana Dieguez

Descripción:
Módulo de comunicación UART para el microcontrolador STM32F4. Este archivo
implementa la configuración e inicialización de los periféricos USART1 y USART2,
permitiendo la transmisión y recepción de datos en comunicación serial. Incluye
funciones para la configuración del baudrate, envío y lectura de datos, así como
la redirección de la salida estándar (printf) a través de USART2 para depuración.
*/

#include "uart.h"
#include "gpio.h"

// ===== DEFINES =====
#define GPIOAEN    (1U<<0)

#define USART2EN   (1U<<17) // APB1
#define USART1EN   (1U<<4)  // APB2

#define CR1_TE     (1U<<3)
#define CR1_RE     (1U<<2)
#define CR1_UE     (1U<<13)

#define SR_TXE     (1U<<7)
#define SR_RXNE    (1U<<5)

#define SYS_FREQ   16000000
#define APB1_CLK   SYS_FREQ
#define APB2_CLK   SYS_FREQ

// =========== Funciones internas ===========
static uint16_t compute_uart_bd(uint32_t clk, uint32_t baud){
    return ((clk + (baud/2U)) / baud);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t clk, uint32_t baud){
    USARTx->BRR = compute_uart_bd(clk, baud);
}

// =========== funciones Para establecer el Baudrate de los USART ===========
void usart2_set_baudrate(uint32_t baud){
    uart_set_baudrate(USART2, APB1_CLK, baud);
    USART2->CR1 |= CR1_UE;
}

void usart1_set_baudrate(uint32_t baud){
    uart_set_baudrate(USART1, APB2_CLK, baud);
    USART1->CR1 |= CR1_UE;
}

// =========== Inicia el USART2 - Pines 2 y 3 como alternativos y configuración de AF7 ===========
void usart2_init(void){
    // Clocks
    RCC->AHB1ENR |= GPIOAEN;
    RCC->APB1ENR |= USART2EN;

    // PA2 Modo Alternativo
    gpioAlternative(GPIOA, 2);

    // PA3 Modo Alternativo
    gpioAlternative(GPIOA, 3);

    // AF7
    GPIOA->AFR[0] |= (1U<<8) | (1U<<9) | (1U<<10);
    GPIOA->AFR[0] &= ~(1U<<11);

    GPIOA->AFR[0] |= (1U<<12) | (1U<<13) | (1U<<14);
    GPIOA->AFR[0] &= ~(1U<<15);

    // TX RX
    USART2->CR1 = CR1_TE | CR1_RE;
}

// =========== Inicia el USART1 - Pines 9 y 10 como alternativos y configuración de AF7 ===========
void usart1_init(void){
    // Clocks
    RCC->AHB1ENR |= GPIOAEN;
    RCC->APB2ENR |= USART1EN;

    // PA9 Modo Alternativo
    gpioAlternative(GPIOA, 9);

    // PA10 Modo Alternativo
    gpioAlternative(GPIOA, 10);

    // AF7
    GPIOA->AFR[1] |= (1U<<4) | (1U<<5) | (1U<<6);
    GPIOA->AFR[1] &= ~(1U<<7);

    GPIOA->AFR[1] |= (1U<<8) | (1U<<9) | (1U<<10);
    GPIOA->AFR[1] &= ~(1U<<11);

    // TX RX
    USART1->CR1 = CR1_TE | CR1_RE;
}

// =========== Manda un Caracter a los USART ===========
void usart2_write(char c){
    while(!(USART2->SR & SR_TXE));
    USART2->DR = c;
}

void usart1_write(char c){
    while(!(USART1->SR & SR_TXE));
    USART1->DR = c;
}

// =========== Lee un caracter desde USART ===========
int usart2_read(void){
    if(USART2->SR & SR_RXNE){
        return USART2->DR;
    }
    return -1;
}

int usart1_read(void){
    if(USART1->SR & SR_RXNE){
        return USART1->DR;
    }
    return -1;
}

// =========== Implementación para Printf ===========
int __io_putchar(int ch){
    usart2_write(ch);
    return ch;
}
