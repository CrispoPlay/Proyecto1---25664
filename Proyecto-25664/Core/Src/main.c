/*
Universidad del Valle de Guatemala
Proyecto 1 - Programación de Microprocesadores
Autor: Cristian Estuardo Orellana Dieguez

main.c
Sistema embebido implementado en un microcontrolador STM32F4 para el control
de una secuencia de LEDs con velocidad variable. El sistema permite la interacción
mediante botones físicos y comunicación UART con un ESP32 y una PC. Incluye
funcionalidades de pausa, ajuste de velocidad y señalización acústica mediante buzzer.
*/

#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "stm32f4xx.h"
#include <stdio.h>

////=========== Define de botones y buzzer ===============
#define BotonSubir 6 //PA6 Boton1
#define BotonBajar 7 //PA7 boton2
#define BotonPausa 8 //PA8 Boton3
#define Buzzer 11 //PA11 Buzzer

//Array que contiene los niveles de velocidad
char *velStr[3] = {"BAJA","MEDIA","ALTA"};

//=========== Habilito puertos y seteo pines ===============
void pinesInit(){
	gpioInit(GPIOA); //Inicio el reloj del puerto A
	gpioInit(GPIOB); //Inicio el reloj del puerto B

	gpioOutputRange(GPIOB,3,6); //Habilito del PB3 al PB6 como output "LEDS"
	gpioOutputRange(GPIOB,8,10); //Habilito del PB8 al PB10 como output "LEDS"
	gpioOutput(GPIOB,0); //Habilito como output el PB0 "LED"
	gpioOutput(GPIOA,Buzzer); //Habilito el pin para el buzzer

	gpioInputRange(GPIOA,BotonSubir,BotonPausa);

	usart2_init(); //Usart2 pines PA2 y PA3
	usart1_init(); //Usart1 pines PA9 y PA10
}
//=========== Establezco el Baudrate de los USART ===============
void baudRateUsart(){
	usart1_set_baudrate(115200); //Baudrate del ESP32
	usart2_set_baudrate(115200); //Baudrate del PC
}
//=========== Secuencia de parpadeo al llega a la velocidad maxima ===============
void avisoMax(void){
    printf("MAX\r\n");
    for(int j = 0; j < 3; j++){
        secuencia(0,0,0,0,1,1,1,1);
        digitalWrite(GPIOA, Buzzer, 1);
        systickDelayMs(50);
        secuencia(0,0,0,0,0,0,0,0);
        digitalWrite(GPIOA, Buzzer, 0);
        systickDelayMs(50);
}}
//=========== Secuencia de parpadeo al llega a la velocidad minima ===============
void avisoMin(void){
    printf("MIN\r\n");
    for(int j = 0; j < 3; j++){
        secuencia(1,1,1,1,0,0,0,0);
        digitalWrite(GPIOA, Buzzer, 1);
        systickDelayMs(50);
        secuencia(0,0,0,0,0,0,0,0);
        digitalWrite(GPIOA, Buzzer, 0);
        systickDelayMs(50);
}}
//=========== Lectura de ESP32 a través de UART1 ===============
void botonesESP32(int *nivelVelocidad, int *pausado, int *velocidades){
    int dato1 = usart1_read();

    if(dato1 != -1){
        char c = (char)dato1;

        if(c == 'A'){
            if(*nivelVelocidad < 2){
                (*nivelVelocidad)++;
                printf("ESP32: SUBIR\r\n");
                printf("Velocidad: %s %d ms\r\n", velStr[*nivelVelocidad], velocidades[*nivelVelocidad]);
                usart1_write('+');
            } else {
                avisoMax();
            }
        }
        else if(c == 'B'){
            if(*nivelVelocidad > 0){
                (*nivelVelocidad)--;
                printf("ESP32: BAJAR\r\n");
                printf("Velocidad: %s %d ms\r\n", velStr[*nivelVelocidad], velocidades[*nivelVelocidad]);
                usart1_write('-');
            } else {
                avisoMin();
            }
        }
        else if(c == 'C'){
            *pausado = !(*pausado);

            if(*pausado){
                printf("ESP32: PAUSA\r\n");
                usart1_write('/');   // 🔹 PAUSA
            } else {
                printf("ESP32: RUN\r\n");
                usart1_write('<');   // 🔹 REANUDAR
            }
        }
    }
}
//=========== Manda caracteres a traveés de Usart1 ===============
void mandarESP32(){
	int dato2 = usart2_read();

	if(dato2 != -1){
	    char c = (char)dato2;

	    usart1_write(c);
	}
}
//=========== Función principal MAIN ===============
int main(void){
	pinesInit(); //Inicializo todos los pines
	baudRateUsart(); // Asigno los baudRate de los usart

	int estado = 0;

	    int velocidades[3] = {1500, 500, 200};
	    int nivelVelocidad = 0;
	    int pausado = 0;
	    uint32_t contador = 0;
	    int lastSubir = 0, lastBajar = 0, lastPausa = 0;

	    while(1){
			//Delay, se ejecutará la cantidad de veces que se asigne el valor
	        systickDelayMs(1);
	        contador++;

	        botonesESP32(&nivelVelocidad, &pausado, velocidades);
	        mandarESP32();

			//Lectura de los botones
	        int subir = gpioReader(GPIOA, BotonSubir);
	        int bajar = gpioReader(GPIOA, BotonBajar);
	        int pausa = gpioReader(GPIOA, BotonPausa);

			// Comprueba el estado del botón Subir y la variable lastSubir
	        if(subir && !lastSubir){
	            if(nivelVelocidad < 2){
	                nivelVelocidad++;
	                printf("Velocidad: %s %d ms\r\n", velStr[nivelVelocidad], velocidades[nivelVelocidad]);
	                usart1_write('+');
	            } else {
	            	avisoMax();

	            }
	        }
	        lastSubir = subir;

			// Comprueba el estado del botón bajar y la variable lastBajar
	        if(bajar && !lastBajar){
	            if(nivelVelocidad > 0){
	                nivelVelocidad--;
	                printf("Velocidad: %s %d ms\r\n", velStr[nivelVelocidad], velocidades[nivelVelocidad]);
	                usart1_write('-');
	            } else {
	            	avisoMin();
	            }
	        }
	        lastBajar = bajar;

			// Comprueba el estado del botón pausar y la variable ultima paisa
	        if(pausa && !lastPausa){
	            pausado = !pausado;
	            if(pausado){
	                    printf("PAUSA\r\n");
	                    usart1_write('/');
	                } else {
	                    printf("RUN\r\n");
	                    usart1_write('<');
	                }
	        }
	        lastPausa = pausa;

	        // Secuencia
	        if(!pausado){
	            if(contador >= velocidades[nivelVelocidad]){
	                contador = 0;

	                switch(estado){
	                    case 0: secuencia(1,1,1,1,1,1,1,1);
	                    		printf("Estado 1\r\n"); break;
	                    case 1: secuencia(1,1,1,1,1,1,1,0);
	                    		printf("Estado 2\r\n"); break;
	                    case 2: secuencia(1,1,1,1,1,1,0,0);
	                    		printf("Estado 3\r\n"); break;
	                    case 3: secuencia(1,1,1,1,1,0,0,0);
	                    		printf("Estado 4\r\n"); break;
	                    case 4: secuencia(1,1,1,1,0,0,0,0);
	                            printf("Estado 5\r\n"); break;
	                    case 5: secuencia(1,1,1,0,0,0,0,0);
	                    		printf("Estado 6\r\n"); break;
	                    case 6: secuencia(1,1,0,0,0,0,0,0);
	                    		printf("Estado 7\r\n"); break;
	                    case 7: secuencia(1,0,0,0,0,0,0,0);
	                    		printf("Estado 8\r\n"); break;
	                }
	                estado++;
	                if(estado > 7) estado = 0;
}}}}




