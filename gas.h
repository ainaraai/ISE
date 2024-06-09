#ifndef __GAS_H
#define __GAS_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include <stdio.h>
#include "defines.h"




void Init_ModGas(void);
osThreadId_t getThIDGas(void);


#endif
