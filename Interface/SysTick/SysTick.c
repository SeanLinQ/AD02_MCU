/******************** (C) COPYRIGHT 2015 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2015/08
* AUTHOR:				SA1
* IC:						SN32F700B
* DESCRIPTION:	SysTick related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2015/08/14	SA1			First release
*
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include "SysTick.h"

#include "GPIO.h"
#include "userApp.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
uint8_t timeoutFlag = 0;
volatile uint8_t FadeProcessFlag = FALSE;
volatile uint8_t SetTimeoutFlag = FALSE;
volatile uint8_t SwitchProcessFlag;
uint8_t Step_Flag = 0;
/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: SysTick_Init
* Description	: Initialization of SysTick timer
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	SysTick_Init (void)
{
	SystemCoreClockUpdate();
	
	__SYSTICK_SET_TIMER_PERIOD(1);			//RELOAD = (system tick clock frequency กั 10 ms)/1000 -1
  //SysTick->LOAD = 6*100 - 1;
	
	__SYSTICK_CLEAR_COUNTER_AND_FLAG;

#if SYSTICK_IRQ == INTERRUPT_METHOD
	SysTick->CTRL = 0x7;			//Enable SysTick timer and interrupt
#else
	SysTick->CTRL = 0x5;			//Enable SysTick timer ONLY
#endif	
}


/*****************************************************************************
* Function		: SysTick_Handler
* Description	: ISR of SysTick interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void SysTick_Handler(void)
{
	static uint32_t SysTickCount = 0;
	static uint32_t Timer0_Count = 0;
	
	__SYSTICK_CLEAR_COUNTER_AND_FLAG;
	
	key_time_power++;
	key_time_dimup++;
	key_time_dimdown++;
	
	if(IsSwitch == TRUE && NormalSwitch == TRUE)
	{
		TimeoutMs++;
		if(TimeoutMs >= 50)
		{
			SwitchProcessFlag = TRUE;		
		}
	}
		
	Timer0_Count++;
	SysTickCount++;
	if(SysTickCount == 10)
	{
		SysTickCount = 0;
		FadeProcessFlag = TRUE;
		Step_Flag = 1;
		
		setTimeout++;
	  // 1 min timeout
		if(setTimeout >= 6000)
		{
				setTimeout = 0;
				SetTimeoutFlag = TRUE;
		}
	}
}


