#ifndef UART_H
#define UART_H

#include "stm32f4xx.h"
static uint16_t compute_uart_bd(uint32_t clk, uint32_t baud);
//Funciones para el Baudrate
void usart1_set_baudrate(uint32_t baud);
void usart2_set_baudrate(uint32_t baud);

//Funciones para iniciar los usart
void usart1_init(void);
void usart2_init(void);

//funciones para escribir en los Usart
void usart1_write(char c);
void usart2_write(char c);

//Funciones para leer en los Usart
int usart1_read(void);
int usart2_read(void);

#endif
