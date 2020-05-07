/******************** (C) COPYRIGHT 2015 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2018/08
* AUTHOR:				SA1
* IC:						SN32F700B
* DESCRIPTION:	PMU related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
*	1.0				2015/08/14	SA1			1. First release
*	1.1				2018/08/24	SA1			1. Remove Deep power-down mode.
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
#include "PMU_drive.h"


/*_____ D E C L A R A T I O N S ____________________________________________*/


/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

/***************************************************************************************************
* Function		: PMU_Setting
* Description	: Setting and enter specified Low power mode
* Input				: mode - specified Low power mode (PMU_SLEEP, PMU_DEEP_SLEEP)
* Output			: None
* Return			: None
* Note				: None
****************************************************************************************************/
void PMU_Setting(uint16_t mode)
{
	SN_PMU->CTRL = mode;

	__WFI();

	SN_PMU->CTRL = 0x0;
}

/*****************************************************************************
* Function		: PMU_Backup_data
* Description	: Fill in backup value in Bakeup registers.
* Input				: None
* Output			: None
* Return			: None
* Note				: User may manage the backup registers on demand.
*****************************************************************************/
void PMU_Backup_data(void)
{
	//User may modify the following backup registers on demand.
	SN_PMU->BKP0 = 0;
	SN_PMU->BKP1 = 1;
	SN_PMU->BKP2 = 2;
	SN_PMU->BKP3 = 3;
	SN_PMU->BKP4 = 4;
	SN_PMU->BKP5 = 5;
	SN_PMU->BKP6 = 6;
	SN_PMU->BKP7 = 7;
}

