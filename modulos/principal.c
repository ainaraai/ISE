#include "principal.h"
#include "SR501.h"
#include "LEDs.h"
#include "rfid.h"
#include "pwm.h"
#include "servomotor.h"
#include "gas.h"
#include "temp.h"
#include "teclado.h"
#include "flash.h"


#define PHY_PWR_DOWN       (1<<11)
#define PHY_ADDRESS        0x00 /* default ADDR for PHY: LAN8742 */



/* Thread IDs */

osThreadId_t TID_Princ;
static osThreadId_t TID_Flash;

//VARIABLES INTERNAS

static uint32_t estado;
static uint32_t flag;
static GPIO_InitTypeDef GPIO_InitStruct;
	   char tag1Id[5];

//DEFINITION OF INTERNAL FUNCTIONS

static void ETH_PhyExitFromPowerDownMode(void);
static void SleepMode_Measure(void);
static void ETH_PhyEnterPowerDownMode(void);
static void ExitSleep(void);



void principal(void *arg) {
	
	//DEFINITION OF THE DIFFERENT STRUCTURED OBJECTS	
	 MSGQUEUE_RC522_t m_rfid;
	 MSGQUEUE_TEMP_t  m_temp;
	 MSGQUEUE_TEC_t   m_teclado;
	 MSGQUEUE_USER_t  m_flashUser1,m_flashUser2,m_flashUser3,m_flashUser4,m_flashUser5,m_flashUser;


    static char generalPSSW[]="3454";
	  static char password[4];
	  static char flashId[5];
	//  static char tag1Id[5];
		static char tag2Id[5];
	  static char tag3Id[5];
	  static char tag4Id[5];
	  static char tag5Id[5];
		static uint8_t cont=0;
	 	static uint8_t contrl=0;
	  static  uint8_t pos;
	  static  uint8_t id[5];
		static  bool find,id1,id2,id3,id4,id5;
	
    id1=true;
		id2=true;
		id3=true;
		id4=true;
		id5=true;
		



	//initModLCD();
	
	while(1) {
		
//		osMessageQueueGet(getMsgTempID(),&m_temp,0U,0U);
//		printf("Temp:%0.2f",m_temp.temp);
		//sprintf(m_lcd.buf,"H: T:%0.2f`C",m_temp.temp);
		   // m_lcd.nLin=1;
		switch(estado){
			
			case FUERA:
				Led_rojo_reset();
				Led_verde_reset();
				flag=0;
				flag=osThreadFlagsWait(PROX | GAS | TEMP ,osFlagsWaitAny,osWaitForever);
			  if(flag & PROX){
					printf("pasa alguien\n");
					Led_verde_set();
					det_rfid_on();
					
				  
					osMessageQueueGet(getMsgRFID(),&m_rfid,0U,2000);
					
					if(m_rfid.id[2]!=NULL){
						  memcpy(id,m_rfid.id,sizeof(uint8_t)*5);
							m_rfid.id[2]=NULL;
							osMessageQueuePut(getMsgRFID(),&m_rfid,NULL,osWaitForever);

							Led_azul_set();
							printf("\n");

							for(uint8_t i=0; i<sizeof(id); i++)
							{
								 printf("%X ",id[i]);
							}
							printf("\n");
							
							
							
							contrl=0;
							
							estado=COMPRUEBA;
							

					}
					det_rfid_off();
					Led_azul_reset();
					Led_verde_reset();
				
				}
				if(flag & GAS){
						printf("Gas leakage");
						osThreadFlagsSet(getModPWMThreadID(),ALERTA);
						estado=ALERTA;	
				}
				if(flag & TEMP){
						printf("FUEGO");
						osThreadFlagsSet(getModPWMThreadID(),ALERTA);
						estado=ALERTA;	
				}
				if(flag & CONSUMO){
						printf("Bajo consumo");
						estado=CONSUMO;	
				}
				break;	
			case COMPRUEBA:
			  flag=0;
				add_user1();
				flag=osThreadFlagsWait(READY,osFlagsWaitAny,osWaitForever);
			//	memcpy(m_flashUser.id,id,5);
			  sprintf(flashId, "%d%d%d%d%d",id[0],id[1],id[2],id[3],id[4]);
			  printf("FId %s",flashId);

				//osMessageQueuePut(getMsgFlashID(), &m_flashUser, NULL, osWaitForever);
//				osThreadFlagsWait(READY,osFlagsWaitAny,osWaitForever);

				osThreadFlagsSet(getModFlashThreadID(),GET_ALL_USERS);
			  osMessageQueueGet(getMsgFlashID(),&m_flashUser1,NULL,5000);
				sprintf(tag1Id, "%d%d%d%d%d",m_flashUser1.id[0],m_flashUser1.id[1],m_flashUser1.id[2],m_flashUser1.id[3],m_flashUser1.id[4]);

				osMessageQueueGet(getMsgFlashID(),&m_flashUser2,NULL,2000);
			//	sprintf(tag2Id, "%s",m_flashUser.id);

			  osMessageQueueGet(getMsgFlashID(),&m_flashUser3,NULL,2000);
			//	sprintf(tag3Id, "%s",m_flashUser.id);

				osMessageQueueGet(getMsgFlashID(),&m_flashUser4,NULL,2000);
			//	sprintf(tag4Id, "%s",m_flashUser.id);

			  osMessageQueueGet(getMsgFlashID(),&m_flashUser5,NULL,2000);
		//		sprintf(tag5Id, "%s",m_flashUser.id);



				sprintf(tag1Id, "%d%d%d%d%d",m_flashUser1.id[0],m_flashUser1.id[1],m_flashUser1.id[2],m_flashUser1.id[3],m_flashUser1.id[4]);
			  printf("Id1 %s",tag1Id);
				sprintf(tag2Id, "%d%d%d%d%d",m_flashUser2.id[0],m_flashUser2.id[1],m_flashUser2.id[2],m_flashUser2.id[3],m_flashUser2.id[4]);
			  printf("Id2 %s",tag2Id);
				sprintf(tag3Id, "%d%d%d%d%d",m_flashUser3.id[0],m_flashUser3.id[1],m_flashUser3.id[2],m_flashUser3.id[3],m_flashUser3.id[4]);
			  printf("Id3 %s",tag3Id);
				sprintf(tag4Id, "%d%d%d%d%d",m_flashUser4.id[0],m_flashUser4.id[1],m_flashUser4.id[2],m_flashUser4.id[3],m_flashUser4.id[4]);
			  printf("Id4 %s",tag4Id);
				sprintf(tag5Id, "%s",m_flashUser5.id);


				//flag=osThreadFlagsWait(READY,osFlagsWaitAny,osWaitForever); 
			  
				//osThreadFlagsWait(READY,osFlagsWaitAny,osWaitForever); 
				//sprintf(tagId, "%s",m_flashUser.id);
				for(int i=0;i<5;i++){
					if(id[i]!=m_flashUser1.id[i]){
						id1=false;			
					}
					if(id[i]!=m_flashUser2.id[i]){
						id2=false;			
					}
					if(id[i]!=m_flashUser3.id[i]){
						id3=false;			
					}
					if(id[i]!=m_flashUser4.id[i]){
						id4=false;			
					}
					if(id[i]!=m_flashUser5.id[i]){
						id5=false;			
					}

				
				}
				if(id1 || id2 || id3 || id4 || id5){
					osThreadFlagsSet(getModPWMThreadID(),ALARMA);
					osThreadFlagsSet(getModServoThreadID(),OPEN_FLAG);
					estado=ALARMA;
				}else{
					Led_rojo_set();
				  estado=FUERA;
				}
				
				
				break;

			case ALARMA:
				Led_azul_set();
				Led_rojo_reset();
				osThreadSuspend(getModRfidID());
			  //osThreadSuspend(getModTempID());
			 

				//osDelay(2000);
			  //osThreadFlagsWait(TECLADO,osFlagsWaitAny,3000);
			  osMessageQueueGet(getMsgTEC(),&m_teclado,0U,30000);
				contrl++;
//				for(int i=0; i<sizeof(id);i++){
//					m_flashUser.id[i]=id[i];
//				}
//		
			  if(strcmp(m_teclado.buf,generalPSSW)==0){
						cont=0;
						printf("PSW: %s\n",m_teclado.buf);
					  Led_verde_set();
						Led_azul_reset();
						osThreadFlagsSet(getModServoThreadID(),CLOSE_FLAG);
						osThreadFlagsSet(getModPWMThreadID(),ZSTOP);
						osThreadResume(getModRfidID());
						//osThreadResume(getModTempID());
						estado=FUERA;
				
				}else{
						Led_rojo_set();
					  cont++;
						printf("PSW: %s,%s\n",generalPSSW,m_teclado.buf);

					  if(cont==3 || sizeof(m_teclado.buf)==0){
							
						Led_azul_reset();
						osThreadFlagsSet(getModPWMThreadID(),ZSTOP);
						osThreadFlagsSet(getModPWMThreadID(),ALERTA);
					  estado=ALERTA;
						osThreadSuspend(getModFlashThreadID());
						cont=0;
						
						}

				}
				

				break;
			case ALERTA:
				Led_rojo_set();
				osMessageQueueGet(getMsgTEC(),&m_teclado,0U,30000);
			
			  if(strcmp(m_teclado.buf,generalPSSW)==0){
					osThreadFlagsSet(getModPWMThreadID(),ZSTOP);
					estado=FUERA;
					osThreadResume(getModRfidID());
				}


				
			
				break;
			case CONSUMO:
				Led_rojo_set();
			  Led_azul_set();
			  osThreadFlagsSet(getModFlashThreadID(),POWER_DOWN);
				ETH_PhyEnterPowerDownMode();
			  SleepMode_Measure();
			  osThreadFlagsWait(CONSUMO,osFlagsWaitAny,osWaitForever);
			  ExitSleep();
			  osThreadFlagsSet(getModFlashThreadID(),POWER_UP);
			  estado=FUERA;		
				Led_rojo_reset();
			  Led_azul_reset();

				break;
		
		
		}
	
	}
	
}



void GPIO_Init_pulsador_azul(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
 void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

static void ExitSleep(void){
			__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();
		__HAL_RCC_GPIOI_CLK_ENABLE();
		__HAL_RCC_GPIOJ_CLK_ENABLE();
		__HAL_RCC_GPIOK_CLK_ENABLE();
	
		HAL_ResumeTick();
    ETH_PhyExitFromPowerDownMode();

}
static void SleepMode_Measure(void)
{
GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();


  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  

}
/**
  * @brief  This function configures the ETH PHY to enter the power down mode
  *         This function should be called before entering the low power mode.
  * @param  None
  * @retval None
  */
static void ETH_PhyEnterPowerDownMode(void)
{
  ETH_HandleTypeDef heth;
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t phyregval = 0; 
   
  /* This part of code is used only when the ETH peripheral is disabled 
	   when the ETH is used in the application this initialization code 
	   is called in HAL_ETH_MspInit() function  ***********************/
	
	/* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2: ETH_MDIO */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1: ETH_MDC */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable the ETH peripheral clocks */
  __HAL_RCC_ETH_CLK_ENABLE();
  
  /* Set ETH Handle parameters */
  heth.Instance = ETH;
  heth.Init.PhyAddress = PHY_ADDRESS;

  /* Configure MDC clock: the MDC Clock Range configuration
	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
  /* MDC: a periodic clock that provides the timing reference for 
	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102;

  /*****************************************************************/
  
  /* ETH PHY configuration in Power Down mode **********************/ 
  
  /* Read ETH PHY control register */
  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
  
  /* Set Power down mode bit */
  phyregval |= PHY_POWERDOWN;
  
  /* Write new value to ETH PHY control register */
  HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
  
  /*****************************************************************/
  
  /* Disable periph CLKs */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_ETH_CLK_DISABLE();
}
/**
  * @brief  This function wakeup the ETH PHY from the power down mode
  *         When exiting from StandBy mode and the ETH is used in the example
  *         its better to call this function at the end of HAL_ETH_MspInit() 
  *         then remove the code that initialize the ETH CLKs ang GPIOs.
  * @param  None
  * @retval None
  */
static void ETH_PhyExitFromPowerDownMode(void)
{
   ETH_HandleTypeDef heth;
   GPIO_InitTypeDef GPIO_InitStruct;
   uint32_t phyregval = 0;
   
  /* ETH CLKs and GPIOs initialization ******************************/
  /* To be removed when the function is called from HAL_ETH_MspInit() when 
     exiting from Standby mode */
	
	/* Enable GPIO clocks*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* Configure PA2 */
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL; 
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure PC1*/
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Enable ETH CLK */
  __HAL_RCC_ETH_CLK_ENABLE();
  /*****************************************************************/
  
  /* ETH PHY configuration to exit Power Down mode *****************/
  /* Set ETH Handle parameters */
  heth.Instance = ETH;
  heth.Init.PhyAddress = PHY_ADDRESS;
  
  /* Configure MDC clock: the MDC Clock Range configuration
	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
  /* MDC: a periodic clock that provides the timing reference for 
	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102; 
	
  /* Read ETH PHY control register */
  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
  
  /* check if the PHY  is already in power down mode */
  if ((phyregval & PHY_POWERDOWN) != RESET)
  {
    /* Disable Power down mode */
    phyregval &= ~ PHY_POWERDOWN;
    
    /* Write value to ETH PHY control register */
    HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
  }
  /*****************************************************************/
}


void InitPrinc(void){
	
	GPIO_Init_pulsador_azul();
	Init_Prox();
	Init_Leds();
	Init_ThMFRC522();
	initModZumbador();
	InitModServo();
	Init_ModGas();
	initModTemp();
	Init_ModTEC();
	TID_Flash=Init_ThFlash();
	estado=FUERA;
	flag=0;


	TID_Princ=osThreadNew (principal, NULL, NULL);
	//add_user1();
 

}
 osThreadId_t getThIDPrinc(void){
    return TID_Princ;
 }
 
