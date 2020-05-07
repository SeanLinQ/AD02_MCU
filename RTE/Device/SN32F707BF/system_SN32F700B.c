/******************************************************************************
 * @file     system_SN32F700B.c
 * @brief    CMSIS Cortex-M0 Device Peripheral Access Layer Source File
 *           for the SONIX SN32F700B Devices
 * @version  V1.0.4
 * @date     2017/12/15
 *
 * @note
 * Copyright (C) 2014-2017 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <stdint.h>
#include <SN32F700B.h>



/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Clock Configuration ----------------------------------
//
//<e> System Clock Configuration
//		<o1.0..2>  SYSCLKSEL (SYS0_CLKCFG)
//					<0=> IHRC
//					<1=> ILRC
//					<2=> EHS X'TAL
//					<3=> ELS X'TAL
//					<4=> PLL
//	
//		<o2> EHS Source Frequency (MHz)
//			<10-16>
//
//	<h> PLL Control Register (SYS0_PLLCTRL)
//			<i> F_CLKOUT = F_VCO / P = (F_CLKIN * M) / P
//			<i>  10 MHz <= F_CLKIN <= 16 MHz
//			<i> 72 MHz <= (F_CLKIN * M) <= 120 MHz
//		<o3> MSEL
//					<0=> M = 4
//					<1=> M = 6
//					<2=> M = 8
//					<3=> M = 10
//					<4=> M = 12
//		<o4> PSEL
//					<0=> P = 2
//					<1=> P = 4
//		<o5> PLL CLKIN Source selection
//					<0=> IHRC
//					<1=> EHS X'TAL
//		<o6> PLL Enable selection
//					<0=> Disable
//					<1=> Enable
//	</h>
//
//		<o7>   AHB Clock Prescaler Register  (SYS0_AHBCP)
//					<0=> SYSCLK/1
//					<1=> SYSCLK/2
//					<2=> SYSCLK/4
//					<3=> SYSCLK/8
//					<4=> SYSCLK/16
//					<5=> SYSCLK/32
//					<6=> SYSCLK/64
//					<7=> SYSCLK/128
//
//		<o8>   CLKOUT selection
//					<0=> Disable
//					<1=> ILRC
//					<2=> ELS X'TAL
//					<4=> HCLK
//					<5=> IHRC
//					<6=> EHS X'TAL
//					<7=> PLL
//		<o9>   CLKOUT Prescaler Register  (SYS1_APBCP1)
//					<0=> CLKOUT selection/1
//					<1=> CLKOUT selection/2
//					<2=> CLKOUT selection/4
//					<3=> CLKOUT selection/8
//					<4=> CLKOUT selection/16
//					<5=> CLKOUT selection/32
//					<6=> CLKOUT selection/64
//					<7=> CLKOUT selection/128
//</e>
*/

#define SYS_CLOCK_SETUP		1
#define SYS0_CLKCFG_VAL		0
#define EHS_FREQ					16//16
#define PLL_MSEL					1
#define PLL_PSEL					0
#define PLL_CLKIN					1
#define PLL_ENABLE				0
#define AHB_PRESCALAR 		0x0
#define CLKOUT_SEL_VAL 		0x0
#define CLKOUT_PRESCALAR 	0x0

/*
//-------- <<< end of configuration section >>> ------------------------------
*/


/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/
#define	IHRC			0
#define	ILRC			1
#define EHSXTAL		2
#define ELSXTAL		3
#define PLL				4


/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __IHRC_FREQ			(12000000UL)
#define __ILRC_FREQ			(16000UL)
#define __ELS_XTAL_FREQ	(32768UL)

#if (SYS_CLOCK_SETUP)
#define SYS0_PLLCTRL_VAL	(PLL_ENABLE<<15) | (PLL_CLKIN<<12) | (PLL_PSEL<<5) |  PLL_MSEL
#endif

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock;	/*!< System Clock Frequency (Core Clock)*/


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
	uint32_t AHB_prescaler;

	switch (SN_SYS0->CLKCFG_b.SYSCLKST)
	{
		case 0:		//IHRC
			SystemCoreClock = __IHRC_FREQ;
			break;
		case 1:		//ILRC
			SystemCoreClock = __ILRC_FREQ;
			break;
		case 2:		//EHS X'TAL
			#if (SYS_CLOCK_SETUP)
			SystemCoreClock = EHS_FREQ * 1000000;
			#else	
			//TODO: User had to assign EHS X'TAL frequency.
			SystemCoreClock = 10000000UL;
			#endif
			break;
		case 3:		//ELS X'TAL
			SystemCoreClock = __ELS_XTAL_FREQ;
			break;	
		case 4: 	//PLL
			#if (SYS_CLOCK_SETUP)
			if (PLL_CLKIN == 0x0)	//IHRC as F_CLKIN
				SystemCoreClock = __IHRC_FREQ * (PLL_MSEL+2) / (PLL_PSEL+1);
			else
				SystemCoreClock = EHS_FREQ * 1000000 * (PLL_MSEL+2) / (PLL_PSEL+1);
			#else
			//TODO: User had to assign PLL output frequency.
			SystemCoreClock = 48000000UL;
			#endif
			break;
		default:
			break;
	}

	switch (SN_SYS0->AHBCP)
	{
		case 0:	AHB_prescaler = 1;	break;
		case 1:	AHB_prescaler = 2;	break;
		case 2:	AHB_prescaler = 4;	break;
		case 3:	AHB_prescaler = 8;	break;
		case 4:	AHB_prescaler = 16;	break;
		case 5:	AHB_prescaler = 32;	break;
		case 6:	AHB_prescaler = 64;	break;
		case 7:	AHB_prescaler = 128;break;
		default: break;	
	}

	SystemCoreClock /= AHB_prescaler;

	//;;;;;;;;; Need for SN32F700B Begin
	if (SystemCoreClock > 24000000)
		SN_FLASH->LPCTRL = 0x5AFA000D;
	else	//SystemCoreClock <= 24000000
	{
		if (SystemCoreClock <= 8000)
			SN_FLASH->LPCTRL = 0x5AFA0002;
		else	//SystemCoreClock > 8000
		{
			if (SystemCoreClock <= 12000000)
				SN_FLASH->LPCTRL = 0x5AFA0000;
			else
				SN_FLASH->LPCTRL = 0x5AFA0003;
		}
	}
	//;;;;;;;;; Need for SN32F700B End

	return;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
#if (SYS_CLOCK_SETUP)

	#if SYS0_CLKCFG_VAL == IHRC			//IHRC
	SN_SYS0->CLKCFG = 0x0;
	while ((SN_SYS0->CLKCFG & 0x70) != 0x0);
	#endif

	#if SYS0_CLKCFG_VAL == ILRC			//ILRC
	SN_SYS0->CLKCFG = 0x1;
	while ((SN_SYS0->CLKCFG & 0x70) != 0x10);
	#endif

	#if (SYS0_CLKCFG_VAL == EHSXTAL)	//EHS XTAL
	SN_SYS0->ANBCTRL_b.EHSFREQ = 1;

	#if (EHS_FREQ > 12)
	SN_FLASH->LPCTRL = 0x5AFA0003;
	#endif
	SN_SYS0->ANBCTRL_b.EHSEN = 1;
	while ((SN_SYS0->CSST & 0x10) != 0x10);
	SN_SYS0->CLKCFG = 0x2;
	while ((SN_SYS0->CLKCFG & 0x70) != 0x20);
	#endif

	#if (SYS0_CLKCFG_VAL == ELSXTAL)	//ELS XTAL
	SN_SYS0->ANBCTRL_b.ELSEN = 1;
	while((SN_SYS0->CSST & 0x4) != 0x4);
	SN_SYS0->CLKCFG = 0x3;
	while ((SN_SYS0->CLKCFG & 0x70) != 0x30);
	#endif

	#if (SYS0_CLKCFG_VAL == PLL)		//PLL
	SN_SYS0->PLLCTRL = SYS0_PLLCTRL_VAL;
	if (PLL_CLKIN == 0x1)	//EHS XTAL as F_CLKIN
	{
		//Enable EHS
		SN_SYS0->ANBCTRL_b.EHSFREQ = 1;
		SN_SYS0->ANBCTRL_b.EHSEN = 1;
		while ((SN_SYS0->CSST & 0x10) != 0x10);
	}
	while ((SN_SYS0->CSST & 0x40) != 0x40);
	SN_FLASH->LPCTRL = 0x5AFA0004;
	SN_FLASH->LPCTRL = 0x5AFA0005;
	SN_FLASH->LPCTRL = 0x5AFA000D;
	SN_SYS0->CLKCFG = 0x4;
	while ((SN_SYS0->CLKCFG & 0x70) != 0x40);
	#endif

	SN_SYS0->AHBCP = AHB_PRESCALAR;

	#if (CLKOUT_SEL_VAL > 0)			//CLKOUT
	SN_SYS1->AHBCLKEN_b.CLKOUTSEL = CLKOUT_SEL_VAL;
	SN_SYS1->APBCP1_b.CLKOUTPRE = CLKOUT_PRESCALAR;
	#endif
#endif //(SYS_CLOCK_SETUP)

}
