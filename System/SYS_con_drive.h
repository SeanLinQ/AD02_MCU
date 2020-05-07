#ifndef __SN32F700B_SYS_CON_H
#define __SN32F700B_SYS_CON_H

/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F700B.h>
#include <SN32F700_Def.h>


/*_____ D E F I N I T I O N S ______________________________________________*/

//Clock
#define	SYS0_CLK_IHRC					0
#define	SYS0_CLK_ILRC					1
#define	SYS0_CLK_EHS					2
#define	SYS0_CLK_ELS					3
#define	SYS0_CLK_PLL					4

//<---------------- SYS0_ANBCTRL ---------------->
//IHRC Enable/Disable
#define	SYS0_IHRC_ENABLE				1
#define	SYS0_IHRC_DISABLE				0
#define	mskSYS0_IHRC_ENABLE			SYS0_IHRC_ENABLE
#define	mskSYS0_IHRC_DISABLE		SYS0_IHRC_DISABLE

//ELS X'tal Enable/Disable
#define	SYS0_ELS_ENABLE					1
#define	SYS0_ELS_DISABLE				0
#define	mskSYS0_ELS_ENABLE			(SYS0_ELS_ENABLE<<2)
#define	mskSYS0_ELS_DISABLE			(SYS0_ELS_DISABLE<<2)

//EHS X'tal Enable/Disable
#define	SYS0_EHS_ENABLE					1
#define	SYS0_EHS_DISABLE				0
#define	mskSYS0_EHS_ENABLE			(SYS0_EHS_ENABLE<<4)
#define	mskSYS0_EHS_DISABLE			(SYS0_EHS_DISABLE<<4)

//EHS X'tal Freq range (Driving)
#define	SYS0_EHS_FREQ_DRIVE_LOW			0											//<=8MHz
#define	SYS0_EHS_FREQ_DRIVE_HIGH		1											//> 8MHz
#define	mskSYS0_EHS_FREQ_DRIVE_LOW	(SYS0_EHS_FREQ_DRIVE_LOW<<5)
#define	mskSYS0_EHS_FREQ_DRIVE_HIGH	(SYS0_EHS_FREQ_DRIVE_HIGH<<5)

//<---------------- SYS0_PLLCTRL ---------------->
//PLL
#define	SYS0_PLL_M4							0
#define	SYS0_PLL_M6							1
#define	SYS0_PLL_M8							2
#define	SYS0_PLL_M10						3
#define	SYS0_PLL_M12						4

#define	SYS0_PLL_P2							0
#define	SYS0_PLL_P4							1

#define	SYS0_PLL_CLOCK_IHRC			0
#define	SYS0_PLL_CLOCK_EHS			1
#define	mskSYS0_PLL_CLOCK_IHRC	(SYS0_PLL_CLOCK_IHRC<<12)
#define	mskSYS0_PLL_CLOCK_EHS		(SYS0_PLL_CLOCK_EHS<<12)

#define	SYS0_PLL_ENABLE					1
#define	SYS0_PLL_DISABLE				0
#define	mskSYS0_PLL_ENABLE			(SYS0_PLL_ENABLE<<15)
#define	mskSYS0_PLL_DISABLE			(SYS0_PLL_DISABLE<<15)

//<---------------- SYS0_CSST ---------------->
//Clock Source Ready Status (R)
#define	SYS0_IHRC_READY					1
#define	SYS0_IHRC_NOT_READY			0
#define	mskSYS0_IHRC_READY			SYS0_IHRC_READY
#define	SYS0_ELS_READY					1
#define	SYS0_ELS_NOT_READY			0
#define	mskSYS0_ELS_READY				(SYS0_ELS_READY<<2)
#define	SYS0_EHS_READY					1
#define	SYS0_EHS_NOT_READY			0
#define	mskSYS0_EHS_READY				(SYS0_EHS_READY<<4)
#define	SYS0_PLL_READY					1
#define	SYS0_PLL_NOT_READY			0
#define	mskSYS0_PLL_READY				(SYS0_PLL_READY<<6)

//<---------------- SYS0_CLKCFG ---------------->
//System Clock Selection
#define	SYS0_SYSCLK_IHRC				0
#define	SYS0_SYSCLK_ILRC				1
#define	SYS0_SYSCLK_EHS					2
#define	SYS0_SYSCLK_ELS					3
#define	SYS0_SYSCLK_PLL					4
#define	mskSYS0_SYSCLK_IHRC			SYS0_SYSCLK_IHRC
#define	mskSYS0_SYSCLK_ILRC			SYS0_SYSCLK_ILRC
#define	mskSYS0_SYSCLK_EHS			SYS0_SYSCLK_EHS
#define	mskSYS0_SYSCLK_ELS			SYS0_SYSCLK_ELS
#define	mskSYS0_SYSCLK_PLL			SYS0_SYSCLK_PLL

//System Clock Switch Status (R)
#define	SYS0_SYSCLK_SW_IHRC			0
#define	SYS0_SYSCLK_SW_ILRC			1
#define	SYS0_SYSCLK_SW_EHS			2
#define	SYS0_SYSCLK_SW_ELS			3
#define	SYS0_SYSCLK_SW_PLL			4
#define	mskSYS0_SYSCLK_SW_IHRC	(SYS0_SYSCLK_SW_IHRC<<4)
#define	mskSYS0_SYSCLK_SW_ILRC	(SYS0_SYSCLK_SW_ILRC<<4)
#define	mskSYS0_SYSCLK_SW_EHS		(SYS0_SYSCLK_SW_EHS<<4)
#define	mskSYS0_SYSCLK_SW_ELS		(SYS0_SYSCLK_SW_ELS<<4)
#define	mskSYS0_SYSCLK_SW_PLL		(SYS0_SYSCLK_SW_PLL<<4)

//<---------------- SYS0_AHBCP ---------------->
#define	SYS0_AHBPRE_DIV1				0
#define	SYS0_AHBPRE_DIV2				1
#define	SYS0_AHBPRE_DIV4				2
#define	SYS0_AHBPRE_DIV8				3
#define	SYS0_AHBPRE_DIV16				4
#define	SYS0_AHBPRE_DIV32				5
#define	SYS0_AHBPRE_DIV64				6
#define	SYS0_AHBPRE_DIV128			7

//<---------------- SYS0_RSTST ---------------->
//System Reset Status
#define	mskSYS0_SW_RESET				0x1
#define	mskSYS0_WDT_RESET				0x2
#define	mskSYS0_LVD_RESET				0x4
#define	mskSYS0_EXT_RESET				0x8
#define	mskSYS0_POR_RESET				0x10

//<---------------- SYS0_LVDCTRL ---------------->
//LVD
#define	SYS0_LVD_ENABLE								1
#define	SYS0_LVD_DISABLE							0
#define	mskSYS0_LVD_ENABLE						(SYS0_LVD_ENABLE<<15)
#define	mskSYS0_LVD_DISABLE						(SYS0_LVD_DISABLE<<15)

#define	SYS0_LVD_RESET_ENABLE					1
#define	SYS0_LVD_RESET_DISABLE				0
#define	mskSYS0_LVD_RESET_ENABLE			(SYS0_LVD_RESET_ENABLE<<14)
#define	mskSYS0_LVD_RESET_DISABLE			(SYS0_LVD_RESET_DISABLE<<14)

#define	SYS0_LVD_INT_1P8V							0
#define	SYS0_LVD_INT_2P0V							1
#define	SYS0_LVD_INT_2P4V							2
#define	SYS0_LVD_INT_2P7V							3
#define	SYS0_LVD_INT_3P0V							4
#define	SYS0_LVD_INT_3P6V							5
#define	mskSYS0_LVD_INT_1P8V					(SYS0_LVD_INT_1P8V<<4)
#define	mskSYS0_LVD_INT_2P0V					(SYS0_LVD_INT_2P0V<<4)
#define	mskSYS0_LVD_INT_2P4V					(SYS0_LVD_INT_2P4V<<4)
#define	mskSYS0_LVD_INT_2P7V					(SYS0_LVD_INT_2P7V<<4)
#define	mskSYS0_LVD_INT_3P0V					(SYS0_LVD_INT_3P0V<<4)
#define	mskSYS0_LVD_INT_3P6V					(SYS0_LVD_INT_3P6V<<4)

#define	SYS0_LVD_RESET_1P8V						0
#define	SYS0_LVD_RESET_2P0V						1
#define	SYS0_LVD_RESET_2P4V						2
#define	SYS0_LVD_RESET_2P7V						3
#define	SYS0_LVD_RESET_3P0V						4
#define	SYS0_LVD_RESET_3P6V						5
#define	mskSYS0_LVD_RESET_1P8V				SYS0_LVD_RESET_1P8V
#define	mskSYS0_LVD_RESET_2P0V				SYS0_LVD_RESET_2P0V
#define	mskSYS0_LVD_RESET_2P4V				SYS0_LVD_RESET_2P4V
#define	mskSYS0_LVD_RESET_2P7V				SYS0_LVD_RESET_2P7V
#define	mskSYS0_LVD_RESET_3P0V				SYS0_LVD_RESET_3P0V
#define	mskSYS0_LVD_RESET_3P6V				SYS0_LVD_RESET_3P6V

//<---------------- SYS0_ANTIEFT ---------------->
//Anti-EFT Ability
#define	SYS0_ANTIEFT_NO								0
#define	SYS0_ANTIEFT_LOW							1
#define	SYS0_ANTIEFT_MEDIUM						2
#define	SYS0_ANTIEFT_HIGH							3

//<---------------- SYS1_AHBCLKEN ---------------->
//Clockout
#define	SYS1_CLOCKOUT_DISABLE					0
#define	SYS1_CLOCKOUT_ILRC						1
#define	SYS1_CLOCKOUT_ELS							2
#define	SYS1_CLOCKOUT_HCLK						4
#define	SYS1_CLOCKOUT_IHRC						5
#define	SYS1_CLOCKOUT_EHS							6
#define	SYS1_CLOCKOUT_PLL							7
#define	mskSYS1_CLOCKOUT_DISABLE			(SYS1_CLOCKOUT_DISABLE<<28)
#define	mskSYS1_CLOCKOUT_ILRC					(SYS1_CLOCKOUT_ILRC<<28)
#define	mskSYS1_CLOCKOUT_ELS					(SYS1_CLOCKOUT_ELS<<28)
#define	mskSYS1_CLOCKOUT_HCLK					(SYS1_CLOCKOUT_HCLK<<28)
#define	mskSYS1_CLOCKOUT_IHRC					(SYS1_CLOCKOUT_IHRC<<28)
#define	mskSYS1_CLOCKOUT_EHS					(SYS1_CLOCKOUT_EHS<<28)
#define	mskSYS1_CLOCKOUT_PLL					(SYS1_CLOCKOUT_PLL<<28)

//<---------------- SYS1_APBCP0 ---------------->
#define	SYS1_CLOCKOUT_DIV1						0
#define	SYS1_CLOCKOUT_DIV2						1
#define	SYS1_CLOCKOUT_DIV4						2
#define	SYS1_CLOCKOUT_DIV8						3
#define	SYS1_CLOCKOUT_DIV16						4



/*_____ M A C R O S ________________________________________________________*/

//Clock Enable/Disable
#define	__SYS0_IHRC_ENABLE								SN_SYS0->ANBCTRL_b.IHRCEN = ENABLE
#define	__SYS0_IHRC_DISABLE								SN_SYS0->ANBCTRL_b.IHRCEN = DISABLE
#define	__SYS0_ELS_ENABLE									SN_SYS0->ANBCTRL_b.ELSEN = ENABLE
#define	__SYS0_ELS_DISABLE								SN_SYS0->ANBCTRL_b.ELSEN = DISABLE
#define	__SYS0_EHS_ENABLE									SN_SYS0->ANBCTRL_b.EHSEN = ENABLE
#define	__SYS0_EHS_DISABLE								SN_SYS0->ANBCTRL_b.EHSEN = DISABLE
#define	__SYS0_PLL_ENABLE									SN_SYS0->PLLCTRL_b.PLLEN = ENABLE
#define	__SYS0_PLL_DISABLE								SN_SYS0->PLLCTRL_b.PLLEN = DISABLE

//EHS Frequency range/driving ability
#define	__SYS0_EHS_FREQ_DRIVE_LOW					SN_SYS0->ANBCTRL_b.EHSFREQ = 0
#define	__SYS0_EHS_FREQ_DRIVE_HIGH				SN_SYS0->ANBCTRL_b.EHSFREQ = 1

//Clock Source Ready Status (R)
#define	__SYS0_CHECK_IHRC_READY_STATUS		(SN_SYS0->CSST & mskSYS0_IHRC_READY)
#define	__SYS0_CHECK_ELS_READY_STATUS			(SN_SYS0->CSST & mskSYS0_ELS_READY)
#define	__SYS0_CHECK_EHS_READY_STATUS			(SN_SYS0->CSST & mskSYS0_EHS_READY)
#define	__SYS0_CHECK_PLL_READY_STATUS			(SN_SYS0->CSST & mskSYS0_PLL_READY)

//System Clock (HCLK) Switch Status (R)
#define	__SYS0_CHECK_SYSCLK_SW_STATUS			(SN_SYS0->CLKCFG & 0x70)

//Clear System Reset Status
#define	__SYS0_CLEAR_SW_RESET							SN_SYS0->RSTST_b.SWRSTF = 0
#define	__SYS0_CLEAR_WDT_RESET						SN_SYS0->RSTST_b.WDTRSTF = 0
#define	__SYS0_CLEAR_LVD_RESET						SN_SYS0->RSTST_b.LVDRSTF = 0
#define	__SYS0_CLEAR_EXT_RESET						SN_SYS0->RSTST_b.EXTRSTF = 0
#define	__SYS0_CLEAR_POR_RESET						SN_SYS0->RSTST_b.PORRSTF = 0

//External Reset pin Enable/Disable
#define	__SYS0_EXT_RESET_ENABLE						SN_SYS0->EXRSTCTRL = 0
#define	__SYS0_EXT_RESET_DISABLE					SN_SYS0->EXRSTCTRL = 1

//SWD pin Enable/Disable
#define	__SYS0_SWD_ENABLE									SN_SYS0->SWDCTRL = 0
#define	__SYS0_SWD_DISABLE								SN_SYS0->SWDCTRL = 1

//Anti-EFT Ability
#define	__SYS0_SET_ANTIEFT_NO							SN_SYS0->ANTIEFT = SYS0_ANTIEFT_NO
#define	__SYS0_SET_ANTIEFT_LOW						SN_SYS0->ANTIEFT = SYS0_ANTIEFT_LOW
#define	__SYS0_SET_ANTIEFT_MEDIUM					SN_SYS0->ANTIEFT = SYS0_ANTIEFT_MEDIUM
#define	__SYS0_SET_ANTIEFT_HIGH						SN_SYS0->ANTIEFT = SYS0_ANTIEFT_HIGH


/*_____ D E C L A R A T I O N S ____________________________________________*/

//Reset status
uint32_t SYS0_GetResetStatus(uint32_t mskRst);

//Clock Switch
void SYS0_EnableIHRC(void);
void SYS0_EnableEHSXtal(uint32_t freq);
void SYS0_EnableELSXtal(void);
void SYS0_EnablePLL(uint32_t Msel, uint32_t Psel, uint32_t clksrc);
void SYS0_SystemClockSwitch(uint32_t sysclk, uint32_t prescale);
void SYS0_SystemCoreClockUpdate(uint32_t hclk);
void SYS1_EnableClockout(uint32_t src, uint32_t prescale);

//LVD
void SYS0_LvdControl(uint32_t LvdEn, uint32_t IntLvl, uint32_t RstEn, uint32_t RstLvl);

#endif	/*__SN32F700B_SYS_CON_H*/
