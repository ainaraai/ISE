#ifndef __SR501_H
#define __SR501_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "cmsis_os2.h"
#include "principal.h"
#include "defines.h"




void Init_Prox(void);
osThreadId_t getModProxThreadID(void);


#endif
