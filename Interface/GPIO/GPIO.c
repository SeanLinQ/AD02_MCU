/******************** (C) COPYRIGHT 2016 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2016/12
* AUTHOR:				SA1
* IC:						SN32F700B
* DESCRIPTION:	GPIO related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
*	1.0				2015/08/14	SA1			1. First release
*	1.2				2016/12/02	SA1			1. Fix GPIO_Interrupt
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
#include "GPIO.h"
#include "userApp.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
extern uint8_t PMU_Status;
uint8_t backup_value = 0;

/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: GPIO_Init
* Description	: GPIO Init
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Init (void)
{
//	// pull-up no use
//	SN_GPIO0->CFG = 0x00A8A8AA;
//	SN_GPIO1->CFG = 0x00AAAA80;
//	SN_GPIO2->CFG = 0x00AAA02A;
//	SN_GPIO3->CFG = 0x00A22AAA;
	/*****key init*******/
	//P2.6 as Input Pull-up   --power
	GPIO_Mode			(GPIO_PORT2, GPIO_PIN6, GPIO_MODE_INPUT);
	GPIO_Config		(GPIO_PORT2, GPIO_PIN6, GPIO_CFG_PULL_UP);	
	//P2.6 as falling edge
	GPIO_P2Trigger(GPIO_PIN6,	GPIO_IS_EDGE, GPIO_IBS_BOTH_EDGE_TRIGGER, GPIO_IEV_FALLING_EDGE);
	GPIO_Interrupt(GPIO_PORT2, GPIO_PIN6,	GPIO_IE_EN);	

	//P1.0 as Input Pull-up    --dim+
	GPIO_Mode			(GPIO_PORT1, GPIO_PIN0, GPIO_MODE_INPUT);
	GPIO_Config		(GPIO_PORT1, GPIO_PIN0, GPIO_CFG_PULL_UP);	
	//P1.0 as falling edge
	GPIO_P1Trigger(GPIO_PIN0,	GPIO_IS_EDGE, GPIO_IBS_BOTH_EDGE_TRIGGER, GPIO_IEV_FALLING_EDGE);
	GPIO_Interrupt(GPIO_PORT1, GPIO_PIN0,	GPIO_IE_EN);	

	//P2.7 as Input Pull-up    --dim-
	GPIO_Mode			(GPIO_PORT2, GPIO_PIN7, GPIO_MODE_INPUT);
	GPIO_Config		(GPIO_PORT2, GPIO_PIN7, GPIO_CFG_PULL_UP);	
	//P2.7 as falling edge
	GPIO_P0Trigger(GPIO_PIN7,	GPIO_IS_EDGE, GPIO_IBS_EDGE_TRIGGER, GPIO_IEV_FALLING_EDGE);
	GPIO_Interrupt(GPIO_PORT2, GPIO_PIN7,	GPIO_IE_EN);	

	/*****DIM init*******/
	//P0.3 as Output High
	GPIO_Mode			(GPIO_PORT0, GPIO_PIN3, GPIO_MODE_OUTPUT);
	//GPIO_Set			(GPIO_PORT0, GPIO_PIN3);
	
	/*****ZCD init*******/
	//P2.0 as Input Repeater-mode
	GPIO_Mode			(GPIO_PORT2, GPIO_PIN0, GPIO_MODE_INPUT);
	GPIO_Config		(GPIO_PORT2, GPIO_PIN0, GPIO_CFG_PULL_UP);	
	//P2.0 as both edge
	GPIO_P2Trigger(GPIO_PIN0,	GPIO_IS_EDGE, GPIO_IBS_EDGE_TRIGGER, GPIO_IEV_FALLING_EDGE);
	GPIO_Interrupt(GPIO_PORT2, GPIO_PIN0,	GPIO_IE_EN);	
	
	/*****uart INT*******/
	GPIO_Mode			(GPIO_PORT3, GPIO_PIN8, GPIO_MODE_INPUT);
	GPIO_Config		(GPIO_PORT3, GPIO_PIN8, GPIO_CFG_PULL_DOWN);
	GPIO_P3Trigger(GPIO_PIN8,	GPIO_IS_EDGE, GPIO_IBS_BOTH_EDGE_TRIGGER, GPIO_IEV_RISING_EDGE );
	GPIO_Interrupt(GPIO_PORT3, GPIO_PIN8,	GPIO_IE_EN);

	NVIC_SetPriority(P2_IRQn,0);
	
	//ESP_I2C_INT
	GPIO_Mode			(GPIO_PORT0, GPIO_PIN8, GPIO_MODE_OUTPUT);
	GPIO_Set			(GPIO_PORT0, GPIO_PIN8);
}

/*****************************************************************************
* Function		: GPIO_Mode
* Description	: set GPIO as input or output
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							mode				- 0 as Input
														1 as output					
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Mode(uint32_t port_number, uint32_t pin_number, uint32_t mode)
{
		uint32_t wGpiomode=0;
		switch(port_number){
			case 0:
				wGpiomode=(uint32_t)SN_GPIO0->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO0->MODE=wGpiomode;				
				wGpiomode=SN_GPIO0->MODE;		//for checlk
			break;
			case 1:
				wGpiomode=(uint32_t)SN_GPIO1->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO1->MODE=wGpiomode;				
				wGpiomode=SN_GPIO1->MODE;		//for checlk
			break;
			case 2:
				wGpiomode=(uint32_t)SN_GPIO2->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO2->MODE=wGpiomode;				
				wGpiomode=SN_GPIO2->MODE;		//for checlk
			break;
			case 3:
				wGpiomode=(uint32_t)SN_GPIO3->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO3->MODE=wGpiomode;				
				wGpiomode=SN_GPIO3->MODE;		//for checlk
			break;
			default:
			break;		
		}
	return;
}

/*****************************************************************************
* Function		: GPIO_Set
* Description	: set GPIO high
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Set(uint32_t port_number, uint32_t pin_number)
{
		switch(port_number){
			case 0:
				SN_GPIO0->BSET|=(1<<pin_number);
			break;
			case 1:
				SN_GPIO1->BSET|=(1<<pin_number);
			break;
			case 2:
				SN_GPIO2->BSET|=(1<<pin_number);
			break;			
			case 3:
				SN_GPIO3->BSET|=(1<<pin_number);
			break;
			default:
			break;			
		}
	return;
}

/*****************************************************************************
* Function		: GPIO_Clr
* Description	: set GPIO low
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Clr(uint32_t port_number, uint32_t pin_number)
{
		switch(port_number){
			case 0:
				SN_GPIO0->BCLR|=(1<<pin_number);
			break;
			case 1:
				SN_GPIO1->BCLR|=(1<<pin_number);
			break;
			case 2:
				SN_GPIO2->BCLR|=(1<<pin_number);
			break;			
			case 3:
				SN_GPIO3->BCLR|=(1<<pin_number);
			break;
			default:
			break;			
		}
	return;
}

/*****************************************************************************
* Function		: GPIO_P0Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger						
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P0Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO0->IS;	
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO0->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO0->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO0->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO0->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO0->IEV=wGpiovalue;
}

/*****************************************************************************
* Function		: GPIO_P1Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger						
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P1Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO1->IS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO1->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO1->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO1->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO1->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO1->IEV=wGpiovalue;
}

/*****************************************************************************
* Function		: GPIO_P2Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger				
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P2Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO2->IS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO2->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO2->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO2->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO2->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO2->IEV=wGpiovalue;
}

/*****************************************************************************
* Function		: GPIO_P3Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger				
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P3Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO3->IS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO3->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO3->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO3->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO3->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO3->IEV=wGpiovalue;		
}

/*****************************************************************************
* Function		: GPIO_Interrupt
* Description	: set GPIO interrupt and NVIC
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							enable			-	0 as disable 
														1 as enable		
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Interrupt(uint32_t port_number, uint32_t pin_number, uint32_t enable)
{
	switch(port_number){
		case 0:
			//check SWD pin
			if ((pin_number == GPIO_PIN10) || (pin_number == GPIO_PIN11)){
				if(SN_SYS0->SWDCTRL!=0x1) return;
			}
			SN_GPIO0->IC=0xFFFF;
			SN_GPIO0->IE&=~(1<<pin_number);
			SN_GPIO0->IE|=(enable<<pin_number);			
			NVIC_ClearPendingIRQ(P0_IRQn);
			NVIC_EnableIRQ(P0_IRQn);
			break;
		case 1:
			SN_GPIO1->IE&=~(1<<pin_number);
			SN_GPIO1->IE|=(enable<<pin_number);
			SN_GPIO1->IC=0xFFFF;
			NVIC_ClearPendingIRQ(P1_IRQn);
			NVIC_EnableIRQ(P1_IRQn);
			break;
		case 2:
			SN_GPIO2->IC=0xFFFF;
			SN_GPIO2->IE&=~(1<<pin_number);
			SN_GPIO2->IE|=(enable<<pin_number);
			NVIC_ClearPendingIRQ(P2_IRQn);
			NVIC_EnableIRQ(P2_IRQn);
			break;
		case 3:
			//check external Reset pin
			if (pin_number == GPIO_PIN6){
				if(SN_SYS0->EXRSTCTRL!=1)	return;
			}
			SN_GPIO3->IC=0xFFFF;
			SN_GPIO3->IE&=~(1<<pin_number);
			SN_GPIO3->IE|=(enable<<pin_number);
			NVIC_ClearPendingIRQ(P3_IRQn);
			NVIC_EnableIRQ(P3_IRQn);			
			break;		
		default:
			break;
	}
	return;
}

/*****************************************************************************
* Function		: GPIO_int_clr
* Description	: set clear interrupt
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,11
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_IntClr(uint32_t port_number, uint32_t pin_number)
{
	switch(port_number){
		case 0:
			SN_GPIO0->IC=(1<<pin_number);
			break;
		case 1:
			SN_GPIO1->IC=(1<<pin_number);
			break;
		case 2:
			SN_GPIO2->IC=(1<<pin_number);
			break;
		case 3:
			SN_GPIO3->IC=(1<<pin_number);
			break;
		default:
			break;		
	}	
}

/*****************************************************************************
* Function		: GPIO_Config
* Description	: set GPIO as pull-up, pull-down, inactive or repeater
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,11
							value				-	0: Pull-up enable 
														1: Pull-down enable
														2: Inactive
														3: Repeate mode
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Config(uint32_t port_number, uint32_t pin_number, uint32_t value)
{
	uint32_t wTemp=0;

	wTemp=pin_number*2;

	switch(port_number){
		case 0:
				SN_GPIO0->CFG&=~(3<<wTemp);
				SN_GPIO0->CFG|=(value<<wTemp);
				break;
		case 1:
				SN_GPIO1->CFG&=~(3<<wTemp);
				SN_GPIO1->CFG|=(value<<wTemp);		
				break;
		case 2:
				SN_GPIO2->CFG&=~(3<<wTemp);
				SN_GPIO2->CFG|=(value<<wTemp);		
				break;
		case 3:
				SN_GPIO3->CFG&=~(3<<wTemp);
				SN_GPIO3->CFG|=(value<<wTemp);	
				break;
		default:
				break;
	}
	return;
}

/*****************************************************************************
* Function		: GPIO_OpenDrain
* Description	: set Open drain
* Input			: port_number - GPIO0
							pin_number	- GPIO_PIN4, GPIO_PIN5
							value				-	0: disable
														1: enable
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_OpenDrain(uint32_t port_number, uint32_t pin_number, uint32_t value)
{
	switch(port_number)
	{
		case 0:
			SN_GPIO0->ODCTRL&=~(1<<pin_number);
			SN_GPIO0->ODCTRL|=(value<<pin_number);
			break;
		default:
			break;
	}	
}

/*****************************************************************************
* Function		: GPIO_IntStatus
* Description	: Check GPIO interrupt status
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,11 
* Output		: None
* Return		: 0 or 1
* Note			: None
*****************************************************************************/
uint32_t GPIO_IntStatus(uint32_t port_number, uint32_t pin_number)
{
	uint32_t wreturn_value=0;
	switch(port_number)
	{
		case 0:
			wreturn_value=(SN_GPIO0->RIS >>pin_number);
		break;
		case 1:
			wreturn_value=(SN_GPIO1->RIS >>pin_number);
		break;
		case 2:
			wreturn_value=(SN_GPIO2->RIS >>pin_number);
		break;
		case 3:
			wreturn_value=(SN_GPIO3->RIS >>pin_number);
		break;
		default:
		break;
	}
	wreturn_value&=0x01;
	return wreturn_value;
}


/*****************************************************************************
* Function		: P0_IRQHandler
* Description	: Set GPIO P0 IRQ
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void P0_IRQHandler(void)
{
  if(GPIO_IntStatus(GPIO_PORT0,GPIO_PIN3)==1)
	{
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN3);
	}
	else if(GPIO_IntStatus(GPIO_PORT0,GPIO_PIN5)==1)
	{
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN5);
	}
	else if(GPIO_IntStatus(GPIO_PORT0,GPIO_PIN8)==1)
	{
   	GPIO_IntClr(GPIO_PORT0,GPIO_PIN8);
	}
	else
	{
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN0);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN1);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN2);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN4);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN6);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN7);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN9);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN10);
		GPIO_IntClr(GPIO_PORT0,GPIO_PIN11);
	}
	SN_GPIO0->IC= 0xFFF;
}

/*****************************************************************************
* Function		: P1_IRQHandler
* Description	: Set GPIO P1 IRQ
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq	void P1_IRQHandler(void)
{
	PMU_Status = 0;
	//SN_GPIO0->DATA_b.DATA9 ^= 0x1;
	if (GPIO_IntStatus(GPIO_PORT1,GPIO_PIN0)==1)
	{
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN0);
	}
	else
	{
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN1);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN2);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN3);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN4);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN5);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN6);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN7);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN8);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN9);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN10);
		GPIO_IntClr(GPIO_PORT1,GPIO_PIN11);
	}
	SN_GPIO1->IC= 0xFFF;
}

uint8_t NL_Flag = 0;
/*****************************************************************************
* Function		: P2_IRQHandler
* Description	: Set GPIO P2 IRQ
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq	void P2_IRQHandler(void)
{
	if (GPIO_IntStatus(GPIO_PORT2,GPIO_PIN0)==1)
	{
		if(NL_Flag == 0)
		{
			NL_Flag = 1;
		}
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN0);
		//Handle ZCD
		ZCD_Process();
	}
	else
	{
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN1);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN2);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN3);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN4);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN5);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN6);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN7);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN8);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN9);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN10);
		GPIO_IntClr(GPIO_PORT2,GPIO_PIN11);
	}
	SN_GPIO2->IC= 0xFFF;
}

/*****************************************************************************
* Function		: P3_IRQHandler
* Description	: Set GPIO P3 IRQ
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq	void P3_IRQHandler(void)
{
	if(GPIO_IntStatus(GPIO_PORT3,GPIO_PIN8)==1)
	{
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN8);
		IsSwitch = TRUE;
		TimeoutMs = 0;
	}
	else
	{
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN0);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN1);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN2);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN3);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN4);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN5);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN6);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN7);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN9);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN10);
		GPIO_IntClr(GPIO_PORT3,GPIO_PIN11);
	}
	SN_GPIO3->IC= 0xFFF;
}

