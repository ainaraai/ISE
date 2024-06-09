#include "gas.h"
#include "lcd.h"
#include "principal.h"

/*--------------------------VARIABLES----------------------------*/
#define RESOLUTION_12B 4096U
#define VREF 3.3f
osThreadId_t tid_ThreadGas;                        // thread id
 
 
/*-------------------PROTOTIPOS DE FUNCIONES---------------------*/
void ThreadGas (void *argument);                   // thread function
void ADC1_pins_F429ZI_config(void);
void ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
void GPIO_Dig_Out(void);
/*---------------------------FUNCIONES---------------------------*/

void GPIO_Dig_Out(void){
/*PG0---->I/0 CN9*/
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);


}

void ADC1_pins_F429ZI_config(){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/*PC0     ------> ADC1_IN10*/
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void ADC_Init_Single_Conversion(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance){
	
   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc->Instance = ADC_Instance;
  hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc->Init.Resolution = ADC_RESOLUTION_12B;
  hadc->Init.ScanConvMode = DISABLE;
  hadc->Init.ContinuousConvMode = ENABLE; //ANTES DISABLE
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc->Init.NbrOfConversion = 1;
  hadc->Init.DMAContinuousRequests = DISABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;

}

float ADC_getVoltage(ADC_HandleTypeDef *hadc, uint32_t Channel)	{
		ADC_ChannelConfTypeDef sConfig = {0};
		HAL_StatusTypeDef status;

		uint32_t raw = 0;
		float voltage = 0;
		 /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = Channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
  {
    return -1;
  }
		
		HAL_ADC_Start(hadc);
		
		do 
			status = HAL_ADC_PollForConversion(hadc, 0); //This funtions uses the HAL_GetTick(), then it only can be executed wehn the OS is running
		while(status != HAL_OK);
		
		raw = HAL_ADC_GetValue(hadc);
		
		voltage = raw*VREF/RESOLUTION_12B; 
		
		return voltage;

}


int Init_ThreadGas (void) {
	
  tid_ThreadGas = osThreadNew(ThreadGas, NULL, NULL);
  if (tid_ThreadGas == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void ThreadGas (void *argument) {
	
	initModLCD();
	
	MSGQUEUE_LCD_t localObject;
	
	float value = 0;
	
	ADC_HandleTypeDef adchandle;
	
	ADC1_pins_F429ZI_config();
	ADC_Init_Single_Conversion(&adchandle , ADC1); //ADC1 configuration
  
	while (1) {
		
					value=ADC_getVoltage(&adchandle , 10 );
								 
				//enviar mensaje al principal y en el principal se vuelve a enviar al lcd y a la web
					sprintf(localObject.buf, "Se ha detectado fuga");
					localObject.nLin=1;
					osMessageQueuePut(getMsgLCDID(), &localObject, 0U, 0U);
					sprintf(localObject.buf, "ad_value:%.2fV",value);
					localObject.nLin=2;
					osMessageQueuePut(getMsgLCDID(), &localObject, 0U, 0U);
				
					printf("Gas leakage!!!\n");
					printf("ad_value:%4.2fV\n",value);
					

						
					
	
    osThreadYield();                            // suspend thread
  }
}
void EXTI0_IRQHandler(void){
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

}


void Init_ModGas(void){
//	Init_ThreadGas();
	GPIO_Dig_Out();

}

osThreadId_t getThIDGas(void){
	return tid_ThreadGas;
}
