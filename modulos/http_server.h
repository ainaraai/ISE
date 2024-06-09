#ifndef __HTTP_SERVER_H
#define __HTTP_SERVER_H

#include "cmsis_os2.h" 
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"
#include <stdio.h>

#define SIZE_MSGQUEUE_LCD			3
#define SIZE_MSGQUEUE_USERS			5
#define SIZE_MSGQUEUE_ALERTS			3

typedef struct {   
	char user1[24];
	char user2[24];
	char user3[24];
	char user4[24];
	char user5[24];
} MSGQUEUE_USUARIOS_t;

typedef struct {   
	char gas[24];
	char fuego[24];
	char piezo[24];
	char pwdAlert[24];
 
} MSGQUEUE_ALERTS_t;




#endif
