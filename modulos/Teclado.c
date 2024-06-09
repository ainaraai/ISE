#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Teclado.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThreadTeclado;                        // thread id
 osMessageQueueId_t mid_MsgQueueTEC;     

void ThreadTeclado (void *argument);                   // thread function

#define MAX_DIGITOS 4

//FUNCIONES

char teclado_read(void);
int Init_ThreadTeclado (void);
void ThreadTeclado (void *argument);

 
int Init_ThreadTeclado (void) {
 	mid_MsgQueueTEC = osMessageQueueNew(3, sizeof(MSGQUEUE_TEC_t), NULL);
  tid_ThreadTeclado = osThreadNew(ThreadTeclado, NULL, NULL);
  if (tid_ThreadTeclado == NULL) {
    return(-1);
  }
 
  return(0);
}




void teclado_init(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/*GPIO PORTS CLOCK ENABLE*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	GPIO_InitStruct.Pin=ROW1_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ROW1_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=ROW2_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ROW2_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=ROW3_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ROW3_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=ROW4_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_NOPULL;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ROW4_PORT,&GPIO_InitStruct);
	
	/*Initialized RESET*/
	HAL_GPIO_WritePin(ROW1_PORT,ROW1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ROW2_PORT,ROW2_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ROW3_PORT,ROW3_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ROW4_PORT,ROW4_PIN,GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin=COL1_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(COL1_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=COL2_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(COL2_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=COL3_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(COL3_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.Pin=COL4_PIN;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(COL4_PORT,&GPIO_InitStruct);
	
//		/*Initialized RESET*/
//	HAL_GPIO_WritePin(COL1_PORT,COL1_PIN,GPIO_PIN_SET);
//	HAL_GPIO_WritePin(COL2_PORT,COL2_PIN,GPIO_PIN_SET);
//	HAL_GPIO_WritePin(COL3_PORT,COL3_PIN,GPIO_PIN_SET);
//	HAL_GPIO_WritePin(COL4_PORT,COL4_PIN,GPIO_PIN_SET);
//	
}

char teclado_read(void){
	char letras[4][4]={{'1','2','3','A'},
										 {'4','5','6','B'},
										 {'7','8','9','C'},
										 {'*','0','#','D'}};
	
	int i=0;
	char valorTecla=0;
	
	for(i=0;i<4;i++){
		if(i==0){
			HAL_GPIO_WritePin(ROW2_PORT,ROW2_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW3_PORT,ROW3_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW4_PORT,ROW4_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW1_PORT,ROW1_PIN,GPIO_PIN_RESET);
			
			osDelay(10); //Bounce time <5ms
			while(HAL_GPIO_ReadPin(COL1_PORT,COL1_PIN)==0){
				valorTecla=letras[0][0]; //1
			}
			while(HAL_GPIO_ReadPin(COL2_PORT,COL2_PIN)==0){
				valorTecla=letras[0][1]; //2
			}
			while(HAL_GPIO_ReadPin(COL3_PORT,COL3_PIN)==0){
				valorTecla=letras[0][2]; //3
			}
			while(HAL_GPIO_ReadPin(COL4_PORT,COL4_PIN)==0){
				valorTecla=letras[0][3]; //A
			}
		}
		if(i==1){
			HAL_GPIO_WritePin(ROW1_PORT,ROW1_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW3_PORT,ROW3_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW4_PORT,ROW4_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW2_PORT,ROW2_PIN,GPIO_PIN_RESET);
			
			osDelay(10); //Bounce time <5ms
			while(HAL_GPIO_ReadPin(COL1_PORT,COL1_PIN)==0){
				valorTecla=letras[1][0]; //4
			}
			while(HAL_GPIO_ReadPin(COL2_PORT,COL2_PIN)==0){
				valorTecla=letras[1][1]; //5
			}
			while(HAL_GPIO_ReadPin(COL3_PORT,COL3_PIN)==0){
				valorTecla=letras[1][2]; //6
			}
			while(HAL_GPIO_ReadPin(COL4_PORT,COL4_PIN)==0){
				valorTecla=letras[1][3]; //B
			}
		}
		if(i==2){
			HAL_GPIO_WritePin(ROW1_PORT,ROW1_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW2_PORT,ROW2_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW4_PORT,ROW4_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW3_PORT,ROW3_PIN,GPIO_PIN_RESET);
			
			osDelay(10); //Bounce time <5ms
			while(HAL_GPIO_ReadPin(COL1_PORT,COL1_PIN)==0){
				valorTecla=letras[2][0]; //7
			}
			while(HAL_GPIO_ReadPin(COL2_PORT,COL2_PIN)==0){
				valorTecla=letras[2][1]; //8
			}
			while(HAL_GPIO_ReadPin(COL3_PORT,COL3_PIN)==0){
				valorTecla=letras[2][2]; //9
			}
			while(HAL_GPIO_ReadPin(COL4_PORT,COL4_PIN)==0){
				valorTecla=letras[2][3]; //C
			}
		}
		if(i==3){
			HAL_GPIO_WritePin(ROW1_PORT,ROW1_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW2_PORT,ROW2_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW3_PORT,ROW3_PIN,GPIO_PIN_SET);
			HAL_GPIO_WritePin(ROW4_PORT,ROW4_PIN,GPIO_PIN_RESET);
			
			osDelay(10); //Bounce time <5ms
			while(HAL_GPIO_ReadPin(COL1_PORT,COL1_PIN)==0){
				valorTecla=letras[3][0]; //*
			}
			while(HAL_GPIO_ReadPin(COL2_PORT,COL2_PIN)==0){
				valorTecla=letras[3][1]; //0
			}
			while(HAL_GPIO_ReadPin(COL3_PORT,COL3_PIN)==0){
				valorTecla=letras[3][2]; //#
			}
			while(HAL_GPIO_ReadPin(COL4_PORT,COL4_PIN)==0){
				valorTecla=letras[3][3]; //D
			}
		}
	}
	return valorTecla;
}
 
void ThreadTeclado (void *argument) {
	
	static uint32_t flags = 0;
	char teclaPulsada=0;
	uint8_t indice=0;
	uint8_t digitos[MAX_DIGITOS];
	MSGQUEUE_TEC_t localObject;
 
	
  while (1){
		//FLAG WAIT PARA CUANDO SE ENTRE EN MODO GESTIONAR ALARMA 
		//O MODO HOME CUANDO SE QUIERE SALIR DE CASA,esto se puede hacer
		//poniendo un boton en la pagina web para indicar que se quiere salir
		//de casa y entonces se activa el flag para comenzar la lectura
		//por teclado o bien por la misma pagina web
		//RESUMEN:
		//-->flag set desde una activación de alarma
		//-->flag set desde boton pulsado en pagina web
		
		teclaPulsada=teclado_read();
		
		if(teclaPulsada!=0){
			if(teclaPulsada<='9' && indice< MAX_DIGITOS){ //si la tecla pulsada es un numero menor o igual que 9 e indice es menor que 4 se añada el numero al array
				Led_rojo_reset();
				digitos[indice]= teclaPulsada;
				indice++;
			}else if(teclaPulsada=='#' && indice==(MAX_DIGITOS)){ //al pulsar # y cuando se hayan pulsado 4 digitos se manda para probar que se guarda bien los digitos al lcd para mostrarlo
//				sprintf(localObject.buf, "Contraseña: %s",digitos);
//				localObject.nLin=1;
//				osMessageQueuePut(getModLCDQueueID(), &localObject, 0U, 0U);
				 indice=0;
				 sprintf(localObject.buf, "%s",digitos);
 			   osMessageQueuePut(mid_MsgQueueTEC, &localObject, 0U, 0U);
				 //osThreadFlagsSet(getThIDPrinc(),TECLADO);
				
			}else if(teclaPulsada=='*'){ // esto no funciona,no me dio tiempo a cambiarlo,queria que pulsado * se reseteara el array digitos donde se esta guardando la contraseña
				//por si al ir pulsando la contraseña se equivocan,
				 indice=0;
				 Led_rojo_set();
				

			}
			else if(teclaPulsada=='D'){ // With this option you can change the general password
				//FLASH

			}
		}
		
    osThreadYield();                            // suspend thread
  }
}

void Init_ModTEC(void){
	Init_ThreadTeclado();
	teclado_init();	 //keyboard initialization

}

osThreadId_t getModTECThreadID(void){

 return tid_ThreadTeclado;
}

osMessageQueueId_t getMsgTEC(void){
	return mid_MsgQueueTEC;

}
