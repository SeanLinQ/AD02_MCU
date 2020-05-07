/******************** (C) COPYRIGHT 2018 SONiX *******************************
* COMPANY:		SONiX
* DATE:				2018/10
* AUTHOR:			SA1
* IC:					SN32F700B
*____________________________________________________________________________
*	REVISION	Date				User		Description
*	1.0				2015/08/25	SA1			1. First version released
*																2. Compatible to CMSIS DFP Architecture in Keil MDK v5.X (http://www.keil.com/dd2/pack/)
*																3. Run HexConvert to generate bin file and show checksum after building.
*	1.2				2016/12/02	SA1			1. Update SYS0_LvdControl in SYS_con_drive.c
*	1.4				2017/07/14	SA1			1. Update system_SN32F700B.c to V1.0.3
*																2. Fix __FLASH_LPM_PLL_MODE in Flash.h
*	2.0				2018/01/26	SA1			1. Add NotPinOut_GPIO_init to set the status of the GPIO which are NOT pin-out to input pull-up.
*																2. Add CodeOption_SN32F700B.s to modify Code option, please modify with Configuration Wizard, 
*																	 and Strongly recommend to keep CS0 for debugging with SN-LINK.
*	2.2				2018/10/05	SA1			1. Remove CodeOption_SN32F700B.s, please contact SONiX FAE if needed.
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
#include <SN32F700_Def.h>

#include "GPIO.h"
#include "CT16B0.h"
#include "CT16B1.h"
#include "CT16B2.h"
#include "SysTick.h"
#include "UART.h"
#include "Flash.h"
#include "PMU_drive.h"
#include "Utility.h"
#include "WDT.h"
#include "userApp.h"
#include "I2C.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

uint8_t PMU_Status = 0;
uint8_t Step_Cnt = 0;

extern volatile uint8_t SwitchProcessFlag;
extern volatile uint8_t wI2C_ReceiveFinishFlag;
extern volatile uint8_t IIC_SendFlag;
extern uint8_t brightValue_bak;

/******************************/
void	NotPinOut_GPIO_init(void);


/*_____ D E F I N I T I O N S ______________________________________________*/
#ifndef	SN32F707B					//Do NOT Remove or Modify!!!
	#error Please install SONiX.SN32F7_DFP.1.4.3.pack or version >= 1.4.3
#endif
#define	PKG		SN32F707B		//User SHALL modify the package on demand (SN32F707B, SN32F706B, SN32F705B, SN32F7051B, SN32F704B, SN32F702B)

/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

/*****************************************************************************
* Function		: Read_Parameter
* Description	: Read parameter 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
static void Read_Parameter(void)
{
	FLASH_Read_Data(ISP_TARGET_ADDR, (uint8_t *)(&Device_Param), sizeof(Device_Param));
	
	// 默认参数
	//if(Device_Param.flag != 0xAA)
	{
		Device_Param.flag = 0xAA;
		Device_Param.led_type = 0x01;
		Device_Param.led_switch = OFF;
		Device_Param.brightness_en = FALSE;
		Device_Param.led_angle = 40;
		Device_Param.led_angle_min = 40;
		Device_Param.bright_value = 10;
		Device_Param.brightness_min = 10;
    	
		//1. Backup data @ISP_TARGET_ADDR if needed	

		//2. Erase page address @ISP_TARGET_ADDR
		//==>Not Necessary to erase for SN32F700B series!!!

		//3. Program content of wFLASH_PGRAM[0] & [1] to User ROM address ISP_TARGET_ADDR & (ISP_TARGET_ADDR+4)
		if (FLASH_ProgramPage(ISP_TARGET_ADDR, sizeof(Device_Param), (uint8_t *)(&Device_Param)) == FAIL){
				while(1);			//Program Fail
		}
		Device_Param.brightness_min = 100;
		FLASH_Read_Data(ISP_TARGET_ADDR, (uint8_t *)(&Device_Param), sizeof(Device_Param));
  }  
}

/*****************************************************************************
* Function		: main
* Description	: Add description of this project Here. 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
int	main (void)
{	
	//User can configure System Clock with Configuration Wizard in system_SN32F700B.c
	SystemInit();
	SystemCoreClockUpdate();				//Must call for SN32F700B, Please do NOT remove!!!

	//Note: User can refer to ClockSwitch sample code to switch various HCLK if needed.

	//1. User SHALL define PKG on demand.
	//2. User SHALL set the status of the GPIO which are NOT pin-out to input pull-up.
	NotPinOut_GPIO_init();

	//--------------------------------------------------------------------------
	//User Code starts HERE!!!
	//WDT_Init();
	
  GPIO_Init();
	I2C0_Init();
	//UART0_Init();
	UART1_Init();
	//MN_CT16B0_Init();
	MN_CT16B1_Init();
	MN_CT16B2_Init();
	SysTick_Init();
	Read_Parameter();

  UT_DelayNms(200);

	//Fre_Cnt_Flag = TRUE;
  backup_value = SN_GPIO3->DATA_b.DATA8;

	while(1)
	{
		WDT_ReloadValue(250);
		if(Step_Flag)
		{
			Step_Flag = 0;
			switch(Step_Cnt++)
			{
				case 0:
					Key_Process();
					break;	
				case 1:
					Fade_Process();
					break;
				case 2:
					UartB_Process();
					break;
		    case 3:
					if(SwitchProcessFlag == TRUE)
					{
						SwitchProcessFlag = FALSE;
						Connect_With_Switch();
					}
					break;
				case 4:
					SetTimeout_Process();
					break;
				
				case 5:
					if(IIC_SendFlag == 1)
					{
						IIC_SendFlag = 0;
						I2C_Write_Frame(I2C_LIGHT_BRIGHTNESS_ACK,2,brightValue_bak);
						//I2C_Process();
					}
        
				default: 
					Step_Cnt = 0;
					if(Device_Param.led_switch == OFF && PMU_Status == 0)
					{
						UT_DelayNms(300);
						SN_GPIO2->DATA_b.DATA1 = 0x0;		// disable ZCD
					  //PMU_Setting(PMU_DEEP_SLEEP);    // deep sleep
						PMU_Status = 1;
					}
					break;
				}
			}
			//PMU_Setting(PMU_SLEEP); 
		}
}


/*****************************************************************************
* Function		: NotPinOut_GPIO_init
* Description	: Set the status of the GPIO which are NOT pin-out to input pull-up. 
* Input				: None
* Output			: None
* Return			: None
* Note				: 1. User SHALL define PKG on demand.
*****************************************************************************/
void	NotPinOut_GPIO_init(void)
{
#if (PKG == SN32F706B)
	//set P0.0~P0.1 to input pull-up
	SN_GPIO0->CFG = 0xAAAAA0;
#elif (PKG == SN32F705B)
	//set P0.0~P0.1 to input pull-up
	SN_GPIO0->CFG = 0x00AAAAA0;
	//set P1.5, P1.8~P1.11 to input pull-up
	SN_GPIO1->CFG = 0x0000A2AA;
	//set P2.5~P2.9 to input pull-up
	SN_GPIO2->CFG = 0x000002AA;
	//set P3.0~P3.3 to input pull-up
	SN_GPIO3->CFG = 0x000AAA00;
#elif (PKG == SN32F7051B)
	//set P0.6~P0.7 to input pull-up
	SN_GPIO0->CFG = 0x00AA0AAA;
	//set P1.5, P1.8~P1.11 to input pull-up
	SN_GPIO1->CFG = 0x0000A2AA;
	//set P2.8~P2.9 to input pull-up
	SN_GPIO2->CFG = 0x0000AAAA;
	//set P3.0~P3.5 to input pull-up
	SN_GPIO3->CFG = 0x000AA000;
#elif (PKG == SN32F704B)
	//set P0.0~P0.1, P0.3 to input pull-up
	SN_GPIO0->CFG = 0x00AAAA20;
	//set P1.5, P1.8~P1.11 to input pull-up
	SN_GPIO1->CFG = 0x0000A2AA;
	//set P2.2~P2.9 to input pull-up
	SN_GPIO2->CFG = 0x0000000A;
	//set P3.0~P3.3 to input pull-up
	SN_GPIO3->CFG = 0x000AAA00;
#elif (PKG == SN32F702B)
	//set P0.3~P0.9 to input pull-up
	SN_GPIO0->CFG = 0x00A0002A;
	//set P1.5, P1.8, P1.10~P1.11 to input pull-up
	SN_GPIO1->CFG = 0x0008A2AA;
	//set P2.3~P2.9 to input pull-up
	SN_GPIO2->CFG = 0x0000002A;
	//set P3.0~P3.5, P3.7~P3.9 to input pull-up
	SN_GPIO3->CFG = 0x00002000;
#endif
}

/*****************************************************************************
* Function		: HardFault_Handler
* Description	: ISR of Hard fault interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void HardFault_Handler(void)
{
	NVIC_SystemReset();
}

