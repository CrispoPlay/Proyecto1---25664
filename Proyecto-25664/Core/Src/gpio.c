#include "stm32f4xx.h"

//Función para leer un pin
int gpioReader(GPIO_TypeDef *port, int Pin){
		return (port -> IDR >> Pin) & 1U;
}

//Función para iniciar el reloj GPIO(Letra)
void gpioInit(GPIO_TypeDef *port){
    if(port == GPIOA) RCC->AHB1ENR |= (1U << 0);
    else if(port == GPIOB) RCC->AHB1ENR |= (1U << 1);
    else if(port == GPIOC) RCC->AHB1ENR |= (1U << 2);
}

//Función para establecer output un rango de pines del mismo  puerto
void gpioOutputRange(GPIO_TypeDef *port,int PinInicial, int PinFinal){
	for(int Pin = PinInicial; Pin <= PinFinal; Pin++){
	port->MODER &= ~(3U << (Pin*2));
	port->MODER |=  (1U << (Pin*2));
	}
}
//Función para establecer input un rango de pines del mismo  puerto
void gpioInputRange(GPIO_TypeDef *port,int PinInicial, int PinFinal){
	for(int Pin = PinInicial; Pin <= PinFinal; Pin++){
	port->MODER &= ~(3U << (Pin*2));
	}
}

//Función para establecer output un pin
void gpioOutput(GPIO_TypeDef *port,int Pin){
	port->MODER &= ~(3U << (Pin*2));
	port->MODER |=  (1U << (Pin*2));
}

//Función para establecer input un pin
void gpioInput(GPIO_TypeDef *port,int Pin){
	port->MODER &= ~(3U << (Pin*2));
}

//Función para establecer alternativo un pin
void gpioAlternative(GPIO_TypeDef *port, int Pin){
	port->MODER &= ~(3U << (Pin*2));
	port->MODER |=  (2U << (Pin*2));
}

//Función para encender o apagar un pin output
void digitalWrite(GPIO_TypeDef *port,int pin, int state){
	if(state == 1){
	port -> BSRR = (1U << pin);}
	else{port -> BSRR = (1U << (pin + 16));}
}

void secuencia(int L1,int L2, int L3, int L4, int L5, int L6, int L7, int L8){
	digitalWrite(GPIOB, 3,L1);
	digitalWrite(GPIOB, 4,L2); 
	digitalWrite(GPIOB, 5,L3); 
	digitalWrite(GPIOB, 6,L4);
	digitalWrite(GPIOB, 8,L5);
	digitalWrite(GPIOB, 9,L6); 
	digitalWrite(GPIOB, 10,L7);
	digitalWrite(GPIOB, 0,L8);
}

