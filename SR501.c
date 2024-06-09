#include "SR501.h"

//INTERNAL VARIABLES

static GPIO_InitTypeDef GPIO_InitStruct;

//FUNCTIONS

void Init_Prox(){
	  
	 
		 HAL_NVIC_EnableIRQ(EXTI4_IRQn);

			__HAL_RCC_GPIOA_CLK_ENABLE();
  	GPIO_InitStruct.Pin=GPIO_PIN_4;

	  GPIO_InitStruct.Mode=GPIO_MODE_IT_FALLING; 
		GPIO_InitStruct.Pull=GPIO_NOPULL; 
		GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH; 
		
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	

}


/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
// __NO_RETURN void Proximidad (void *arg) {
//  //static char    buf[24];
//	//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
//  //while(1) {
//    //PROXIMIDAD//
////		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)==1){
////			osThreadFlagsSet(getThIDPrinc(),PROX);
////		}

// // }
//}
// 
void EXTI4_IRQHandler(void){
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

}


