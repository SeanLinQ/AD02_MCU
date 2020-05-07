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
#include "CT16B1.h"
#include "CT16B2.h"

#include "userApp.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
volatile uint32_t iwCT16B2_IrqEvent = 0x00; //The bitmask usage of iwCT16Bn_IrqEvent is the same with CT16Bn_RIS

void	CT16B2_Init (void);
void	CT16B2_NvicEnable (void);
void	CT16B2_NvicDisable (void);

/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: CT16B2_Init
* Description	: Initialization of CT16B2 timer
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	CT16B2_Init (void)
{
	//Enable P_CLOCK for CT16B2.
	__CT16B2_ENABLE;		

}

/*****************************************************************************
* Function		: CT16B2_NvicEnable
* Description	: Enable CT16B2 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	CT16B2_NvicEnable (void)
{
	NVIC_ClearPendingIRQ(CT16B2_IRQn);
	NVIC_EnableIRQ(CT16B2_IRQn);
	NVIC_SetPriority(CT16B2_IRQn,0);			// Set interrupt priority (default)
}

/*****************************************************************************
* Function		: CT16B2_NvicDisable
* Description	: Disable CT16B2 timer interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void	CT16B2_NvicDisable (void)
{
	NVIC_DisableIRQ(CT16B2_IRQn);
}

/*****************************************************************************
* Function		: CT16B2_IRQHandler
* Description	: ISR of CT16B2 interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void CT16B2_IRQHandler(void)
{
	//MR9
	if (SN_CT16B2->MCTRL_b.MR9IE)				//Check if MR9 IE enables?
	{	
		  // enable GPIO0.3 to GPIO mode
		  SN_CT16B2->PWMCTRL = mskCT16_PWM2EN_EN|mskCT16_PWM2MODE_FORCE_0|mskCT16_PWM2N_GPIO;
			SN_GPIO0->DATA_b.DATA3 = 0;
	
			//SN_CT16B2->PWMCTRL_b.PWM2MODE = 3;
			iwCT16B2_IrqEvent |= mskCT16_MR9IF;		
	}
	SN_CT16B2->IC = 0xFFFF;	//Clear MR9 match interrupt status
}

/*****************************************************************************
* Function		: MN_CT16B2_Init
* Description	: Init CT16B2 PWM. 
* Input			: None
* Output		: None
* Return		: Nones
* Note			: None
*****************************************************************************/
void MN_CT16B2_Init(void)
{
	CT16B2_Init();

	SN_CT16B2->PRE = 3;
	
	//Set period 8.3ms, duty 100%
	SN_CT16B2->MR9 = 24490;	

	SN_CT16B2->MR2 = 0;					//Set MR3 match value as 24490 (60Hz)

	//Set MR9 match interrupt 
	SN_CT16B2->MCTRL = mskCT16_MR9RST_EN;	

	//Enable PWM function, IOs and select the PWM modes
	SN_CT16B2->PWMCTRL = mskCT16_PWM2EN_EN|mskCT16_PWM2MODE_2|mskCT16_PWM2N_MODE1;	//Enable PWM3N function, IO and select as PWM mode 1		

	//Set MR9 match interrupt and TC rest 
	SN_CT16B2->MCTRL = (mskCT16_MR9IE_EN|mskCT16_MR9STOP_EN);
	
	//Set CT16B2 as the Center-aligned mode1.
	SN_CT16B2->TMRCTRL = mskCT16_CRST|mskCT16_CM_EDGE_UP;
	
	//Wait until timer reset done.
	while (SN_CT16B2->TMRCTRL & mskCT16_CRST);	
	
	//Let TC start counting.
	SN_CT16B2->TMRCTRL |= mskCT16_CEN_EN;	
	
	CT16B2_NvicEnable();
}



