/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2018 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server_CGI.c
 * Purpose: HTTP Server CGI Module
 * Rev.:    V6.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "LEDs.h"
#include "http_server.h"
#include "JSON.h"
//#include "Board_LED.h"                  // ::Board Support:LED

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic push
#pragma  clang diagnostic ignored "-Wformat-nonliteral"
#endif

// http_server.c
extern uint16_t AD_in (uint32_t ch);
extern uint8_t  get_button (void);

extern bool consumo;
extern char add_text[3][20+1];
extern osThreadId_t TID_Usuarios;
extern uint32_t cnt;

extern RTC_HandleTypeDef RtcHandle;
extern RTC_TimeTypeDef stimestructureget;
extern RTC_DateTypeDef sdatestructureget;

extern osMessageQueueId_t mid_MsgQueueUsers; 
extern osMessageQueueId_t mid_MsgQueueAlerts; 

MSGQUEUE_USUARIOS_t users;
MSGQUEUE_ALERTS_t alerts;

extern char delete_text[5][20+1];



// Local variables.
static uint8_t P2;
static uint8_t ip_addr[NET_ADDR_IP6_LEN];
static char    ip_string[40];
char time[24];
char date[24];

char user1[24];
char user2[24];
char user3[24];
char user4[24];
char user5[24];

char alert1[24];
char alert2[24];
char alert3[24];


// My structure of CGI status variable.
typedef struct {
  uint8_t idx;
  uint8_t unused[3];
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

// Process query string received by GET request.
void netCGI_ProcessQuery (const char *qstr) {
  netIF_Option opt = netIF_OptionMAC_Address;
  int16_t      typ = 0;
  char var[40];

  do {
    // Loop through all the parameters
    qstr = netCGI_GetEnvVar (qstr, var, sizeof (var));
    // Check return string, 'qstr' now points to the next parameter

    switch (var[0]) {
      case 'i': // Local IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_Address;       }
        else               { opt = netIF_OptionIP6_StaticAddress; }
        break;

      case 'm': // Local network mask
        if (var[1] == '4') { opt = netIF_OptionIP4_SubnetMask; }
        break;

      case 'g': // Default gateway IP address
        if (var[1] == '4') { opt = netIF_OptionIP6_DefaultGateway; }
        else               { opt = netIF_OptionIP6_DefaultGateway; }
        break;

      case 'p': // Primary DNS server IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_PrimaryDNS; }
        else               { opt = netIF_OptionIP6_PrimaryDNS; }
        break;

      case 's': // Secondary DNS server IP address
        if (var[1] == '4') { opt = netIF_OptionIP4_SecondaryDNS; }
        else               { opt = netIF_OptionIP6_SecondaryDNS; }
        break;
      
      default: var[0] = '\0'; break;
    }

    switch (var[1]) {
      case '4': typ = NET_ADDR_IP4; break;
      case '6': typ = NET_ADDR_IP6; break;

      default: var[0] = '\0'; break;
    }

    if ((var[0] != '\0') && (var[2] == '=')) {
      netIP_aton (&var[3], typ, ip_addr);
      // Set required option
      netIF_SetOption (NET_IF_CLASS_ETH, opt, ip_addr, sizeof(ip_addr));
    }
  } while (qstr);
}

// Process data received by POST request.
// Type code: - 0 = www-url-encoded form data.
//            - 1 = filename for file upload (null-terminated string).
//            - 2 = file upload raw data.
//            - 3 = end of file upload (file close requested).
//            - 4 = any XML encoded POST data (single or last stream).
//            - 5 = the same as 4, but with more XML data to follow.
void netCGI_ProcessData (uint8_t code, const char *data, uint32_t len) {
  char var[40],passw[12];
   

  if (code != 0) {
    // Ignore all other codes
    return;
  }

  P2 = 0;
  consumo = false;
  if (len == 0) {
    // No data or all items (radio, checkbox) are off
    Led_verde_reset();
    //LED_SetOut (P2);
    return;
  }
  passw[0] = 1;
  do {
    // Parse all parameters
    data = netCGI_GetEnvVar (data, var, sizeof (var));
    if (var[0] != 0) {
      // First character is non-null, string exists
      if (strncmp (var, "id=", 3) == 0) {
        strcpy (add_text[0], var+3);
        osThreadFlagsSet (TID_Usuarios, 0x01);
      }else if (strncmp (var, "nombre=", 7) == 0) {
        strcpy (add_text[1], var+7);
        osThreadFlagsSet (TID_Usuarios, 0x01);
      }else if (strncmp (var, "password=", 9) == 0) {
        strcpy (add_text[2], var+9);
        osThreadFlagsSet (TID_Usuarios, 0x01);
      }else if (strncmp (var, "pwdAlert=", 9) == 0) {
        strcpy (add_text[0], var+9);
        osThreadFlagsSet (TID_Usuarios, 0x200);
      }else if(strcmp (var, "ctrlConsumo=Enable") == 0){
        consumo=true;
      }else if(strcmp (var, "ctrlConsumo=Disable") == 0){
        consumo=false;
      
      //else if (strcmp (var, "fuegoCheck=on") == 0) {
//        P2 |= 0x50;
//        osThreadFlagsSet (TID_Usuarios, 0x03);
//       
//      }else if (strcmp (var, "gasCheck=on") == 0) {
//        P2 |= 0x60;
//        osThreadFlagsSet (TID_Usuarios, 0x04);
//       
//      }else if (strcmp (var, "lcdCheck=on") == 0) {
//        P2 |= 0x70;
//        osThreadFlagsSet (TID_Usuarios, 0x05);
//       
//      }else if (strncmp (var, "pwdExit=",8) == 0) {
//				strcpy (add_text[0], var+8);
//        osThreadFlagsSet (TID_Usuarios, 0x06);
			}else if (strcmp (var, "user1Del=on") == 0) {
        P2 |= 0x01;
        osThreadFlagsSet (TID_Usuarios, 0x02);
       
      
			}else if (strcmp (var, "user2Del=on") == 0) {
        P2 |= 0x02;
        osThreadFlagsSet (TID_Usuarios, 0x04);
       
      }else if (strcmp (var, "user3Del=on") == 0) {
        P2 |= 0x04;
        osThreadFlagsSet (TID_Usuarios, 0x08);
       
      }else if (strcmp (var, "user4Del=on") == 0) {
        P2 |= 0x08;
        osThreadFlagsSet (TID_Usuarios, 0x10);
       
      }else if (strcmp (var, "user5Del=on") == 0) {
        P2 |= 0x10;
        osThreadFlagsSet (TID_Usuarios, 0x20);
       
      }
    }
  } while (data);
}

// Generate dynamic web data from a script line.
uint32_t netCGI_Script (const char *env, char *buf, uint32_t buflen, uint32_t *pcgi) {
  int32_t socket;
  netTCP_State state;
  NET_ADDR r_client;
  const char *lang;
  uint32_t len = 0U;
  uint8_t id;
  static uint32_t adv;
  netIF_Option opt = netIF_OptionMAC_Address;
  int16_t      typ = 0;
  
  
  
//        // Leer la entrada estándar para obtener los datos enviados desde el navegador
//    char *data1 = NULL;
//    size_t len1 = 0;
//    ssize_t read;
//    while ((read = fread(&data1, &len1, stdin)) != -1) {
//        // Procesar los datos, aquí parsearías la cadena JSON para obtener el valor de showButtonPressed
//        cJSON *root = cJSON_Parse(data);
//        if (root == NULL) {
//            printf("Error parsing JSON data\n");
//            return 1;
//        }

//        cJSON *showButtonPressedJSON = cJSON_GetObjectItemCaseSensitive(root, "showButtonPressed");
//        if (cJSON_IsBool(showButtonPressedJSON)) {
//            cJSON_bool showButtonPressed = cJSON_IsTrue(showButtonPressedJSON);
//            printf("showButtonPressed: %s\n", showButtonPressed ? "true" : "false");
//            // Aquí puedes realizar acciones basadas en el valor de showButtonPressed
//        } else {
//            printf("showButtonPressed is not a boolean\n");
//        }

//        cJSON_Delete(root);
//    }

//    // Liberar memoria
//    free(data);

	MSGQUEUE_USUARIOS_t users;
	
  switch (env[0]) {
    // Analyze a 'c' script line starting position 2
      
     case 'b':
      // LED control from 'led.cgi'
      if (env[2] == 'c') {
        // Select Control
        len = (uint32_t)sprintf (buf, &env[4], consumo ?     ""     : "selected",
                                               consumo ? "selected" :    ""     );
        break;
      }
     break;
				
    case 'l':
      // Consume CheckBox from consumo.cgi
      id = env[2] - '0';
      if (id > 0) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;
		
    case 'c':
      // Consume CheckBox from consumo.cgi
      id = env[2] - '0';
      if (id > 0) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;
      
    case 'a':
      // Consume CheckBox from consumo.cgi
      id = env[2] - '0';
      if (id > 0) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;

			
    case 'd':
      // Consume CheckBox from consumo.cgi
      id = env[2] - '0';
      if (id > 0) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;
      
    case 'e':
      // Consume CheckBox from consumo.cgi
      id = env[2] - '0';
      if (id > 0) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;
		case 'x':
      // control CheckBox from control.cgi
      id = env[2] - '0';
      if (id > 1) {
        id = 0;
      }
      id = (uint8_t)(1U << id);
      len = (uint32_t)sprintf (buf, &env[4], (P2 & id) ? "checked" : "");
      break;


    case 'f':
      // Usuarios Module control from 'usuarios.cgi'
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf, &env[4], add_text[0]);
          break;
        case '2':
          len = (uint32_t)sprintf (buf, &env[4], add_text[1]);
          break;
        case '3':
          len = (uint32_t)sprintf (buf, &env[4], add_text[2]);
          break;
      }
      break;
			
		case 'm':
      //Salida Module control from 'salida.cgi'
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf, &env[4], add_text[0]);
          break;
      }
      break;
			
		case 'g':
      //Salida Module control from 'salida.cgi'
      switch (env[2]) {
        case '1':
          len = (uint32_t)sprintf (buf, &env[4], add_text[0]);
          break;
      }
      break;
    case 'j':
      // AD Input from 'ad.cgi'
      switch (env[2]) {
        case '1':
          
          HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
          sprintf(time, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
          len = (uint32_t)sprintf (buf, &env[4],time);
          break;
        case '2':
          HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
          sprintf(date, "%.2d-%.2d-%.2d",  sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
          len = (uint32_t)sprintf (buf, &env[4], date);
          break;
      }
      break;
			
    case 'h':
      // AD Input from 'ad.cgi'
		  //osMessageQueueGet(mid_MsgQueueUsers, &users, NULL, osWaitForever);
			osThreadFlagsSet (TID_Usuarios, 0x40);
			osMessageQueueGet(mid_MsgQueueUsers, &users, NULL, osWaitForever);
      switch (env[2]) {
        case '1':
          sprintf(user1, "%-20s", users.user1);
          len = (uint32_t)sprintf (buf, &env[4],user1);
          break;
        case '2':
          sprintf(user2, "%-20s", users.user2);
          len = (uint32_t)sprintf (buf, &env[4], user2);
          break;
        case '3':
          sprintf(user3, "%-20s",  users.user3);
          len = (uint32_t)sprintf (buf, &env[4], user3);
          break;
        case '4':
          sprintf(user4, "%-20s",  users.user4);
          len = (uint32_t)sprintf (buf, &env[4], user4);
          break;
        case '5':
          sprintf(user5, "%-20s",  users.user5);
          len = (uint32_t)sprintf (buf, &env[4], user5);
          break;				
      }
      break;			
     
		  case 'k':
				switch (env[2]) {
					case '1':
						sprintf(user1, "%-20s", "user1");
						len = (uint32_t)sprintf (buf, &env[1],user1);
						break;
					case '2':
						sprintf(user2, "%-20s", "user2");
						len = (uint32_t)sprintf (buf, &env[1],user2);
						break;
				}
      break;	
		
     case 'v':
			osThreadFlagsSet (TID_Usuarios, 0x100);
			osMessageQueueGet(mid_MsgQueueAlerts, &alerts, NULL, osWaitForever);
      switch (env[2]) {
        case '1':
          sprintf(alert1, "%-24s", alerts.fuego);
          len = (uint32_t)sprintf (buf, &env[4],alert1);
          break;
        case '2':
          sprintf(alert2, "%-20s", alerts.gas);
          len = (uint32_t)sprintf (buf, &env[4], alert2);
          break;
        case '3':
          sprintf(alert3, "%-20s",  alerts.piezo);
          len = (uint32_t)sprintf (buf, &env[4], alert3);
          break;			
      }
     break;		
			
		case 'i':
      // time.cgx          
          HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
          sprintf(time, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
          len = (uint32_t)sprintf (buf, &env[1],time);
      break;
  }
  return (len);
}

#if      defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma  clang diagnostic pop
#endif
