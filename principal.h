#ifndef __PRIN_H
#define __PRIN_H

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "defines.h"
#include <string.h>

 void InitPrinc(void);
 osThreadId_t getThIDPrinc(void);
#endif
