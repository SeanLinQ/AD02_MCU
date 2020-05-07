/******************** (C) COPYRIGHT 2015 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2015/08
* AUTHOR:				SA1
* IC:						SN32F700B
* DESCRIPTION:	CT16B0 related functions.
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
#include "CT16B0.h"
#include "WDT.h"

#include "userApp.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
volatile uint32_t iwCT16B0_IrqEvent = 0x00; //The bitmask usage of iwCT16Bn_IrqEvent is the same with CT16Bn_RIS
//volatile uint8_t SwitchProcessFlag;

void	CT16B0_Init (void);
void	CT16B0_NvicEnable (void);
void	CT16B0_NvicDisable (void);

/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: CT16B0_Init
* Description	: Initialization of CT16B0 timer
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	CT16B0_Init (void)
{
	//Enable P_CLOCK for CT16B0.
	__CT16B0_ENABLE;

	//Set MR0 value for 1 ms period  @ HCLK = 12MHz
	SN_CT16B0->MR0 = 150;
	
	//Set MR0 match as TC stop, and enable MR0 interrupt
	SN_CT16B0->MCTRL = (mskCT16_MR0RST_EN|mskCT16_MR0IE_EN);

	//Set CT16B0 as the up-counting mode.
	SN_CT16B0->TMRCTRL = (mskCT16_CRST|mskCT16_CM_EDGE_UP);

	//Wait until timer reset done.
	while (SN_CT16B0->TMRCTRL & mskCT16_CRST);

	//Let TC start counting.
	SN_CT16B0->TMRCTRL |= mskCT16_CEN_EN;
	
	//Enable CT16B0 NVIC interrupt
	NVIC_ClearPendingIRQ(CT16B0_IRQn);
	CT16B0_NvicEnable();
}

/*****************************************************************************
* Function		: CT16B0_NvicEnable
* Description	: Enable CT16B0 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	CT16B0_NvicEnable (void)
{
	NVIC_ClearPendingIRQ(CT16B0_IRQn);
	NVIC_EnableIRQ(CT16B0_IRQn);
	//NVIC_SetPriority(CT16B0_IRQn,0);		// Set interrupt priority (default)
}

/*****************************************************************************
* Function		: CT16B0_NvicEnable
* Description	: Disable CT16B0 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	CT16B0_NvicDisable (void)
{
	NVIC_DisableIRQ(CT16B0_IRQn);
}

/*****************************************************************************
* Function		: CT16B0_IRQHandler
* Description	: ISR of CT16B0 interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void CT16B0_IRQHandler(void)
{ 
	TimeoutMs++;
 
	SN_CT16B0->IC = 0xFFFF;	//Clear MR0 match interrupt status

}

/*****************************************************************************
* Function		: MN_CT16B0_Init
* Description	: Init CT16B0 timer. 
* Input			: None
* Output		: None
* Return		: None
* Note			: Connect LEDs to P2.0 and P2.1
*****************************************************************************/
void MN_CT16B0_Init(void)
{
	CT16B0_Init();
}

