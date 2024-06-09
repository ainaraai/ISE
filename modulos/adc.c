
#include "stm32f4xx_hal.h"
#include "adc.h"

#define ADC_RESOLUTION        12        /* Number of A/D converter bits       */
#define MSGQUEUE_OBJECTS 6                     // number of Message Queue Objects


static ADC_HandleTypeDef hadc1;

static volatile uint8_t  AD_done;       /* AD conversion done flag            */
 
osMessageQueueId_t mid_MsgQueue;                // message queue id
 
osThreadId_t tid_Thread_MsgQueue1;              // thread id 1
 
void Thread_MsgQueue1 (void *argument);         // thread function 1

/**
  \fn          int32_t ADC_Initialize (void)
  \brief       Initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Initialize (void) {

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();

  /* ADC3 GPIO Configuration: PF3 -> ADC3_IN9 */
		GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	
	 /* ADC3 GPIO Configuration: PF5 -> ADC3_IN15 */
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* Peripheral clock enable */
  __HAL_RCC_ADC3_CLK_ENABLE();

  /* Configure the global features of the ADC
    (Clock, Resolution, Data Alignment and number of conversion) */
  hadc1.Instance = ADC3;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc1); 
	


  AD_done = 0;

  return 0;
}

/**
  \fn          int32_t ADC_Uninitialize (void)
  \brief       De-initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Uninitialize (void) {

  /* Peripheral clock disable */
  __HAL_RCC_ADC3_CLK_DISABLE();

  /* ADC3 GPIO Configuration: PF3 -> ADC3_IN9 */
  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3);

	/* ADC3 GPIO Configuration: PF5 -> ADC3_IN15 */
  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_15);


  return 0;
}

/**
  \fn          int32_t ADC_StartConversion (void)
  \brief       Start conversion
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_StartConversion (void) {
  __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
  HAL_ADC_Start(&hadc1);

  AD_done = 0;

  return 0;
}


/**
  \fn          int32_t ADC_ConversionDone (void)
  \brief       Check if conversion finished
  \returns
   - \b  0: conversion finished
   - \b -1: conversion in progress
*/
int32_t ADC_ConversionDone (void) {
  HAL_StatusTypeDef status;

  status = HAL_ADC_PollForConversion(&hadc1, 0);
  if (status == HAL_OK) {
    AD_done = 1;
    return 0;
  } else {
    AD_done = 0;
    return -1;
  }
}



/**
  \fn          int32_t ADC_GetValue (void)
  \brief       Get converted value
  \returns
   - <b> >=0</b>: converted value
   - \b -1: conversion in progress or failed
*/
int32_t ADC_GetValue (void) {
  HAL_StatusTypeDef status;
  int32_t val;

  if (AD_done == 0) {
    status = HAL_ADC_PollForConversion(&hadc1, 0);
    if (status != HAL_OK) return -1;
  } else {
    AD_done = 0;
  }

  val = (int32_t)HAL_ADC_GetValue(&hadc1);

  return val;
}



/**
  \fn          uint32_t ADC_GetResolution (void)
  \brief       Get resolution of Analog-to-Digital Converter
  \returns     Resolution (in bits)
*/
uint32_t ADC_GetResolution (void) {
  return ADC_RESOLUTION;
}

float ADC_getValue (uint32_t ch) {
  int32_t val = 0;
   ADC_ChannelConfTypeDef sConfig;
  if (ch == 1) {
    
      /* Configure the selected ADC channel */
  sConfig.Channel = 9; //CANAL 9 DEL ADC
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }

	if (ch == 2) {
    
    		  /* Configure the selected ADC channel */
  sConfig.Channel = 15; //CANAL 15 DEL ADC
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }
	
  //return (val)*3.3/4096;
	return (uint16_t)val;
}

int Init_MsgQueue (void) {
 
  mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_ADC_t), NULL);
  if (mid_MsgQueue == NULL) {
    ; // Message Queue object not created, handle failure
  }
 
  tid_Thread_MsgQueue1 = osThreadNew(Thread_MsgQueue1, NULL, NULL);
  if (tid_Thread_MsgQueue1 == NULL) {
    return(-1);
  }

  return(0);
}
osMessageQueueId_t getMsgADCID(void){
	return mid_MsgQueue;
}
 
void Thread_MsgQueue1 (void *argument) {
  MSGQUEUE_ADC_t adc;
	
 
  while (1) {
     // Insert thread code here...
		adc.consumo=ADC_getValue(1);
		adc.pres=ADC_getValue(2);

    osMessageQueuePut(mid_MsgQueue, &adc, 0U, 0U);
    osThreadYield();                                            // suspend thread
  }
}

void Init_ModADC(void){
	ADC_Initialize();
	Init_MsgQueue();
	
}