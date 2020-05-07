#ifndef __SN32F700B_RTC_H
#define __SN32F700B_RTC_H


/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F700B.h>


/*_____ D E F I N I T I O N S ______________________________________________*/
#define SECOND			0
#define RTC_MODE		SECOND					//SECOND

//RTC enable
#define mskRTC_RTCEN_DISABLE		0		
#define mskRTC_RTCEN_ENABLE			1		

//RTC Clock source
#define RTC_CLKSEL_ILRC					0
#define RTC_CLKSEL_ELS					1
#define RTC_CLKSEL_EHS					3

//RTC Interrupt Enable/Disable
#define	RTC_IE_ENABLE						1
#define RTC_IE_DISABLE					0

#define mskRTC_SECIE_ENABLE			RTC_IE_ENABLE
#define mskRTC_SECIE_DISABLE		RTC_IE_DISABLE

#define mskRTC_SECIF						(0x1<<0)								//Interrupt flag for Second		

#define mskRTC_SECIC						mskRTC_SECIF 


/*_____ M A C R O S ________________________________________________________*/
//LCD HCLK Enable/Disable
#define	__RTC_ENABLE_RTCHCLK					(SN_SYS1->AHBCLKEN |= (1<<23))
#define	__RTC_DISABLE_RTCHCLK					(SN_SYS1->AHBCLKEN &= ~(1<<23))

//RTC Enable/Disable
#define	__RTC_ENABLE						(SN_RTC->CTRL |= mskRTC_RTCEN_ENABLE)
#define	__RTC_SECCNTV(value)		(SN_RTC->SECCNTV = value)


/*_____ D E C L A R A T I O N S ____________________________________________*/
void RTC_Init(void);
void RTC_SelectClockSource(uint32_t src);
void RTC_NvicEnable(void);
void RTC_NvicDisable(void);

#endif	/*__SN32F700B_RTC_H*/
