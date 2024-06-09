/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "lcd.h"
#include "rtc.h"
#include "LEDs.h"
#include "adc.h"
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "http_server.h"
#include "sntp.h"
//#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern uint16_t AD_in          (uint32_t ch);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);


uint8_t cnt;
extern uint8_t  get_button (void);


/*---------------------VARIABLES USUARIOS------------*/

#define SIZE_MSGQUEUE_USERS			5
//#define SIZE_MSGQUEUE_CONTROL			3

char add_text[3][20+1];


osMessageQueueId_t mid_MsgQueueUsers;  
osMessageQueueId_t mid_MsgQueueAlerts;  

char delete_text[5][20+1];
char events_text[3][20+1];

uint32_t flags = 0;

MSGQUEUE_LCD_t localObject;


/*---------------------------------------------------*/



bool consumo;
char lcd_text[2][20+1];

/* Buffers used for displaying Time and Date */
extern char aShowTime[50];
extern char aShowDate[50];

/* Thread IDs */
osThreadId_t TID_Usuarios;
osThreadId_t TID_Led;
osThreadId_t TID_RTC;

/* Thread declarations */
static void BlinkLed (void *arg);
static void Usuarios  (void *arg);

ADC_HandleTypeDef adchandle;
__NO_RETURN void app_main (void *arg);


 extern osTimerId_t tim_id2_alarma;                            // timer id ALARMA
/* RTC handler declaration */
extern RTC_HandleTypeDef RtcHandle;

static uint32_t exec2_SNTP;
osTimerId_t tim_id2_SNTP; 
osTimerId_t tim_id2_servidor; 
static uint32_t exec2_servidor;
///* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
    //handler definition
		//val=ADC_getVoltage(&adchandle ,10); //get values from channel 10->ADC123_IN10
   ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
   val = ADC_GetValue();
  }
  return ((uint16_t)val);
}

//* Read digital inputs */
uint8_t get_button (void) {
//  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Usuarios, 0x08);
  }
}


//static void Timer_Callback_Servidor (void const *arg) {
//  
//  if(encendidoRojo){
//    LED_Off(2);
//    encendidoRojo=0;
//  }else{
//    LED_On(2);
//    encendidoRojo=1;
//  }
//	cont_servidor++;
//	if (cont_servidor >= 8){
//		osTimerStop(tim_id2_servidor);
//		cont_servidor =0;
//		
//	}
//	
//}
//// Example: Create and Start timers
//int Init_Timers_Servidor (void) {
//  osStatus_t status;                            // function return status

//  // Create periodic timer
//  exec2_servidor = 2U;
//  tim_id2_servidor = osTimerNew((osTimerFunc_t)&Timer_Callback_Servidor, osTimerPeriodic, &exec2_servidor, NULL);
//  if (tim_id2_servidor != NULL) {  // Periodic timer created
//		
//    if (status != osOK) {
//      return -1;
//    }
//  }
//  return NULL;
//}

static void Timer2_Callback_SNTP (void const *arg) {
	
    
	  get_time ();
		//osTimerStart(tim_id2_servidor,250U);

//  if (SNTP_Server == true){
//	  get_time ();
//	  osTimerStart(tim_id2_led,200);
//	}
//	else {
//	  get_time_2 ();
//	  osTimerStart(tim_id2_led,200);
//		
//	}
}

int Init_Timers_SNTP (void) {
  osStatus_t status;                            // function return status
  
 
  // Create periodic timer
  exec2_SNTP = 2U;

  tim_id2_SNTP = osTimerNew((osTimerFunc_t)&Timer2_Callback_SNTP, osTimerPeriodic, &exec2_SNTP, NULL);
  if (tim_id2_SNTP != NULL) {  // Periodic timer created
    // start timer with periodic 1000ms interval
    status = osTimerStart(tim_id2_SNTP, 30000U);    //30 segundos        
    if (status != osOK) {
      return -1;
    }
  }
  return NULL;
}


/*----------------------------------------------------------------------------
  Thread 'RTC': RTC handler
 *---------------------------------------------------------------------------*/
void Thread_RTC (void *argument){
	/*Initialization*/
	RTC_init();
	if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }
	LED_Initialize();
	//Init_Timers_Alarma();
	Init_Timers_SNTP ();
	get_time ();
	RTC_Alarm();
	osDelay(5000);
	
	while(1){
		RTC_CalendarShow(aShowTime, aShowDate);
		osDelay(100);
//    if(ENCENDERTIMALARM == osThreadFlagsWait(ENCENDERTIMALARM,osFlagsWaitAny,10U)){
//      osThreadFlagsClear(ENCENDERTIMALARM);
//      osTimerStart(tim_id2_alarma,250U);
//    }
		//RTC_Alarm();
		
	}
}


/*----------------------------------------------------------------------------
  Thread 'Usuarios':User handler
 *---------------------------------------------------------------------------*/
//COLA
void Init_MsgQueue_Users (void) {
 
  mid_MsgQueueUsers = osMessageQueueNew(SIZE_MSGQUEUE_USERS , sizeof(MSGQUEUE_USUARIOS_t), NULL);

}
void Init_MsgQueue_Alerts (void) {
 
  mid_MsgQueueAlerts = osMessageQueueNew(SIZE_MSGQUEUE_ALERTS , sizeof(MSGQUEUE_ALERTS_t), NULL);

}


////COLA
//void Init_MsgQueue_Control (void) {
// 
//  mid_MsgQueueControl = osMessageQueueNew(SIZE_MSGQUEUE_CONTROL , sizeof(MSGQUEUE_USUARIOS_t), NULL);

//}






static __NO_RETURN void Usuarios (void *arg) {
   (void)arg;
  
	
  initModLCD();
	Init_MsgQueue_Users();
	Init_MsgQueue_Alerts();
  //Init_MsgQueue_Control();
  
   MSGQUEUE_USUARIOS_t users;
	MSGQUEUE_ALERTS_t alerts;
	
  //MSGQUEUE_CONTROL_t control;
  
  strcpy(delete_text[0], "lorena1");
	strcpy(delete_text[1], "lorena2");
	strcpy(delete_text[2], "lorena3");
	strcpy(delete_text[3], "lorena4");
	strcpy(delete_text[4], "lorena5");
  
	sprintf (users.user1, "%-20s", delete_text[0]);
	sprintf (users.user2, "%-20s", delete_text[1]);
	sprintf (users.user3, "%-20s", delete_text[2]);
	sprintf (users.user4, "%-20s", delete_text[3]);
	sprintf (users.user5, "%-20s", delete_text[4]);


	sprintf (alerts.gas, "%-20s", "No hay fuga de gas");
	sprintf (alerts.fuego, "%-20s", "No hay fuego");
	sprintf (alerts.piezo, "%-24s", "No se ha detectado");
    
  while(1) {
    flags = osThreadFlagsWait(0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20| 0x40 | 0x80 | 0x100 |0x200, osFlagsWaitAny, osWaitForever);
    if (flags & 0x01) { //funciona añadir usuarios
        sprintf(localObject.buf,"%-20s", add_text[0]);
				localObject.nLin=1;
				osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
				sprintf(localObject.buf, "%-20s %-20s",add_text[1],add_text[2]);
				localObject.nLin=2;
				osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    } 
    if (flags & 0x02) {
        sprintf(localObject.buf,"%s", "user1 deleted");
        localObject.nLin=1;
        osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }
    if (flags & 0x04) {
        sprintf(localObject.buf,"%s", "user2 deleted");
        localObject.nLin=1;
        osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }
    if (flags & 0x08) {
        sprintf(localObject.buf,"%s", "user3 deleted");
        localObject.nLin=1;
        osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }
    if (flags & 0x10) {
        sprintf(localObject.buf,"%s", "user4 deleted");
        localObject.nLin=1;
        osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }
    if (flags & 0x20) {
        sprintf(localObject.buf,"%-20s", "user5 delete");
        localObject.nLin=1;
        osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }
    if (flags & 0x40) {
				osMessageQueuePut(mid_MsgQueueUsers, &users, 0U, 0U);
    }
		if (flags & 0x80) { //funciona añadir usuarios
        sprintf(localObject.buf,"%-20s", add_text[0]);
				localObject.nLin=1;
				osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }

    if (flags & 0x100) {
				osMessageQueuePut(mid_MsgQueueAlerts, &alerts, 0U, 0U);
    }
		if (flags & 0x200) { //funciona añadir usuarios
        sprintf(localObject.buf,"%-20s", add_text[0]);
				localObject.nLin=1;
				osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
    }

    
//   strcpy(delete_text[0], "lorena1v2");
//    strcpy(delete_text[1], "lorena2v2");
//    strcpy(delete_text[2], "lorena3v2");
//    strcpy(delete_text[3], "lorena4v2");
//    strcpy(delete_text[4], "lorena5v2");
// sprintf (users.user1, "%-20s", delete_text[0]);
//    sprintf (users.user2, "%-20s", delete_text[1]);
//    sprintf (users.user3, "%-20s", delete_text[2]);
//    sprintf (users.user4, "%-20s", delete_text[3]);
//    sprintf (users.user5, "%-20s", delete_text[4]);
   


    

  }
}







/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  netInitialize ();


	ADC_Initialize();
	
	//initModLCD();

  TID_Usuarios = osThreadNew (Usuarios,  NULL, NULL);
	TID_RTC = osThreadNew (Thread_RTC,  NULL, NULL);

  osThreadExit();
}
