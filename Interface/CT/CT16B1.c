/******************** (C) COPYRIGHT 2015 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2015/08
* AUTHOR:				SA1
* IC:						SN32F700B
* DESCRIPTION:	CT16B1 related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2015/08/24	SA1			First release
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
#include <SN32F700B.h>
#include "CT16.h"
#include "CT16B1.h"

#include "userApp.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
volatile uint32_t iwCT16B1_IrqEvent = 0x00; //The bitmask usage of iwCT16Bn_IrqEvent is the same with CT16Bn_RIS
uint32_t Timer1_Value = 0;
uint8_t IIC_HeatTimer = 0;

void CT16B1_Init (void);
void CT16B1_NvicEnable (void);
void CT16B1_NvicDisable (void);

/*_____ D E F I N I T I O N S ______________________________________________*/

/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: CT16B1_Init
* Description	: Initialization of CT16B1 timer
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void CT16B1_Init (void)
{
	//Enable P_CLOCK for CT16B1.
	__CT16B1_ENABLE;					

	//Set MR0 value for 50us period  @ HCLK = 12MHz
	SN_CT16B1->MR0 = 600;

	//Set MR0 match as TC stop, and enable MR0 interrupt
	SN_CT16B1->MCTRL = (mskCT16_MR0RST_EN|mskCT16_MR0IE_EN);
  
	//Set CT16B1 as the up-counting mode.
	SN_CT16B1->TMRCTRL = (mskCT16_CRST|mskCT16_CM_EDGE_UP);

	//Wait until timer reset done.
	while (SN_CT16B1->TMRCTRL & mskCT16_CRST);

	//Let TC start counting.
	SN_CT16B1->TMRCTRL |= mskCT16_CEN_EN;
	
	//Enable CT16B1 NVIC interrupt
	NVIC_ClearPendingIRQ(CT16B1_IRQn);
	NVIC_SetPriority(CT16B1_IRQn,0);
	CT16B1_NvicEnable();
}

/*****************************************************************************
* Function		: CT16B1_NvicEnable
* Description	: Enable CT16B1 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void CT16B1_NvicEnable (void)
{
	NVIC_ClearPendingIRQ(CT16B1_IRQn);
	NVIC_EnableIRQ(CT16B1_IRQn);
	//NVIC_SetPriority(CT16B1_IRQn,0);			// Set interrupt priority (default)
}

/*****************************************************************************
* Function		: CT16B1_NvicDisable
* Description	: Enable CT16B1 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void CT16B1_NvicDisable (void)
{
	NVIC_DisableIRQ(CT16B1_IRQn);
}

/*****************************************************************************
* Function		: CT16B1_IRQHandler
* Description	: ISR of CT16B1 interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void CT16B1_IRQHandler(void)
{
	static uint16_t count = 0;

	Timer1_Value++;
	count++;
	if(count >= 1000)
	{
		count = 0;
		IIC_HeatTimer++;
	}
	// Communication for IIC timeout, need to reset ESP32
	if(IIC_HeatTimer >= 200)
	{
		IIC_HeatTimer = 0;
	}
	
	SN_CT16B1->IC = 0xFFFF;	//Clear MR0 match interrupt status
}

/*****************************************************************************
* Function		: MN_CT16B1_Init
* Description	: Init CT16B1 timer. 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void MN_CT16B1_Init(void)
{
	CT16B1_Init();	
}

