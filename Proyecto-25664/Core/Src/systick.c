/*
Universidad del Valle de Guatemala
Proyecto 1 - Programación de Microprocesadores
Autor: Cristian Estuardo Orellana Dieguez

Descripción:
Módulo de temporización basado en el temporizador SysTick del microcontrolador
STM32F4. Este archivo implementa una función de retardo en milisegundos mediante
el uso de un contador interno, permitiendo la generación de pausas precisas
necesarias para el control de tiempos en sistemas embebidos.
*/

#include "stm32f4xx.h"

#define SYSTICK_LOAD_VAL      16000
#define CTRL_ENABLE           (1U<<0)
#define CTRL_CLKSRC           (1U<<2)
#define CTRL_COUNTFLAG        (1U<<16)

void systickDelayMs(int delay)
{
    /*Reload with number of clocks per millisecond*/
    SysTick->LOAD = SYSTICK_LOAD_VAL;

    /*Clear systick current value register */
    SysTick->VAL = 0;

    /*Enable systick and select internal clk src*/
    SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;

    for(int i=0; i<delay; i++){

        /*Wait until the COUNTFLAG is set*/
        while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
    }

    SysTick->CTRL = 0;
}

