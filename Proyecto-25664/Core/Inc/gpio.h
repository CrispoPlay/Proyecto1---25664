#ifndef GPIO_H_
#define GPIO_H_
#include "stm32f4xx.h"

int gpioReader(GPIO_TypeDef *port, int Pin);
void gpioInit(GPIO_TypeDef *port);
void gpioOutputRange(GPIO_TypeDef *port,int PinInicial, int PinFinal);
void gpioInputRange(GPIO_TypeDef *port,int PinInicial, int PinFinal);
void gpioOutput(GPIO_TypeDef *port,int Pin);
void gpioInput(GPIO_TypeDef *port,int Pin);
void gpioAlternative(GPIO_TypeDef *port, int Pin);
void digitalWrite(GPIO_TypeDef *port,int Pin, int state);
void secuencia(int L1,int L2, int L3, int L4, int L5, int L6, int L7, int L8);

#endif /* GPIO_H_ */
