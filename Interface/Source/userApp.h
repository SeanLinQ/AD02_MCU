/****************************************Copyright (c)*************************
**                               版权所有 (C), INNOTECH
**
**
**--------------文件信息-------------------------------------------------------
**文   件   名: userApp.h
**描        述: 用户执行头文件
**使 用 说 明 : 此文件下函数可供其他文件调用
**
**
**--------------当前版本修订---------------------------------------------------
** 版  本: 
** 日　期: 2019年8月15日
** 描　述: 
		   
**-----------------------------------------------------------------------------
******************************************************************************/
#ifndef __USERAPP_H
#define __USERAPP_H

/* Includes ------------------------------------------------------------------*/
#include <SN32F700B.h>
#include "string.h"
#include "SN32F700_Def.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define MCU_VER "V1.0.0"

enum{
	KEY_SHORT_DOWN,
	KEY_SHORT_UP,
	KEY_LONG_DOWN,
	KEY_LONG_UP,
	KEY_ADD_SHORT_DOWN,
	KEY_ADD_SHORT_UP,
	KEY_ADD_LONG_DOWN,
	KEY_ADD_LONG_5S_DOWN,
	KEY_ADD_LONG_UP,
	KEY_DEC_SHORT_DOWN,
	KEY_DEC_SHORT_UP,
	KEY_DEC_LONG_DOWN,
	KEY_DEC_LONG_5S_DOWN,
	KEY_DEC_LONG_UP,
	KEY_NULL
};

enum{
	STEP0,
	STEP1,
	STEP2,
	STEP3,
	STEP4,
	STEP5,
};

typedef struct {
	uint8_t flag;                                                                 //存储标志(是否调过亮度)
	uint8_t led_type;                                                             //led类型
  uint8_t led_switch;                                                           //开关
	uint8_t brightness_en;                                                        //调亮度使能 
	uint32_t led_angle;                                                           //导通角度
	uint32_t led_angle_min;                                                       //最低导通角                                               
  uint32_t bright_value;                                                        //亮度值
	uint32_t brightness_min;                                                      //最低亮度 
}DEVICE_PARAM;

/* Exported macro ------------------------------------------------------------*/
//指示灯数据
#define LED1_ON          GPIO_Clr     (GPIO_PORT1, GPIO_PIN1)
#define LED2_ON          GPIO_Clr     (GPIO_PORT1, GPIO_PIN2)
#define LED3_ON          GPIO_Clr     (GPIO_PORT1, GPIO_PIN3) 
#define LED4_ON          GPIO_Clr     (GPIO_PORT3, GPIO_PIN9)
#define LED5_ON          GPIO_Clr     (GPIO_PORT0, GPIO_PIN8)
#define LED_POWER_ON     GPIO_Clr     (GPIO_PORT0, GPIO_PIN9)

#define LED1_OFF         GPIO_Set			(GPIO_PORT1, GPIO_PIN1)
#define LED2_OFF         GPIO_Set			(GPIO_PORT1, GPIO_PIN2)
#define LED3_OFF         GPIO_Set			(GPIO_PORT1, GPIO_PIN3) 
#define LED4_OFF         GPIO_Set			(GPIO_PORT3, GPIO_PIN9)
#define LED5_OFF         GPIO_Set			(GPIO_PORT0, GPIO_PIN8)
#define LED_POWER_OFF    GPIO_Set			(GPIO_PORT0, GPIO_PIN9)

#define ZERO           0

#define ON             1
#define OFF            0

//按键数据
#define KEY_POWER      0x01  	
#define KEY_DIM_UP     0x02  
#define KEY_DIM_DOWN   0x04  

#define KEY_TIMEZERO   50
#define KEY_TIMESHORT  1000
#define KEY_TIMELONG   2000
#define KEY_TIMELONG_A 5000

//dim数据
#define DIM_L          SN_GPIO0->BSET|=(1<<3)  //GPIO_Set			(GPIO_PORT0, GPIO_PIN4)
#define DIM_H          SN_GPIO0->BCLR|=(1<<3)  //GPIO_Clr     (GPIO_PORT0, GPIO_PIN4)

#define MIN_BR         40
#define MAX_BR				 140

//串口数据
#define FRAME_HEAD_1            0xAA       

#define UART_MODE               0x00
#define UART_SWITCH             0x9A
#define UART_BRIGHT_VALUE       0x9B
#define UART_BRIGHT_VALUE_MIN   0x9C
#define UART_BRIGHT_KEY_PRESS   0x9D

#define MODE_NORMAL      0     //正常工作模式
#define MODE_BRIGHT_SET  1     //最低亮度设置模式

#define LOAD_TYPE_DIMMER     		0x00
#define LOAD_TYPE_SWITCH        0x01

//Assign ISP Address
#define	ISP_TARGET_ADDR		0x5000//0x1FE0				//Word-alignment

// key parameter
extern uint16_t key_time_power;
extern uint16_t key_time_dimup;
extern uint16_t key_time_dimdown;

extern uint16_t TimeoutMs;
extern uint8_t IsSwitch; 

extern uint8_t workMode;   //0:正常工作模式 1:最低亮度设置模式
extern DEVICE_PARAM Device_Param;
extern uint32_t light_angle;
extern uint8_t angleBackup;     //保存设置最低亮度时的当前亮度值
extern uint32_t angleMinBackup; //保存设置最低亮度时的当前最低亮度值
extern uint32_t brightnessBackup;
extern uint16_t setTimeout;     //设置最低亮度模式超时

extern uint16_t pwm_delay;
extern uint8_t SetFinishFlag;

extern uint8_t dim_fade_add;
extern uint8_t dim_fade_dec;
extern uint8_t fade_add_count ;
extern uint8_t fade_dec_count ;
extern uint8_t report_count;

extern uint8_t NormalSwitch;
extern uint8_t backup_value;

extern uint8_t Fre_Cnt_Flag;

/* Exported functions ------------------------------------------------------- */
extern void UartB_ReceiveData(uint8_t RcvData);
void Connect_With_Switch(void);
extern void ZCD_Process(void);
extern void Key_Process(void);
void Fade_Process(void);
void SetTimeout_Process(void);
extern void UartB_Process(void);
extern void Led_Process(void);
extern void I2C_Process(void);

void I2C_Write_Frame(const uint8_t cmd, uint8_t len, uint8_t value);

uint8_t I2C_CalcFCS( const uint8_t *msg_ptr, uint8_t len );

extern void UartB_SendData(uint8_t cmd, uint8_t sync_data);
extern void Dimmer_Save_Prmtr(void);
static void system_factory_default(void);

#endif


