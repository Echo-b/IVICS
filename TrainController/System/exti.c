#include "stm32f10x.h"
#include "L298N.h"
void EXTI_wInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 			
	EXTI_InitTypeDef EXTI_InitStructure;			
	NVIC_InitTypeDef NVIC_InitStructure;			

	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO,ENABLE);
												
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
    NVIC_Init(&NVIC_InitStructure);	
    
	
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;					
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;				
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;						
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;						
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;					
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;								
    EXTI_Init(&EXTI_InitStructure);
    

}
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET) 	
	{
        
     do
	{

		L298N_BACK(15000);

	}while(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3));
			L298N_RUN(0);
		
		EXTI_ClearITPendingBit(EXTI_Line0);     
	}  
}



