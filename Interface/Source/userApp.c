/****************************************Copyright (c)*************************
**                               版权所有 (C), INNOTECH
**
**
**--------------文件信息-------------------------------------------------------
**文   件   名: userApp.c
**描        述: 用户执行接口
**使 用 说 明 : 此文件下函数可供其他文件调用
**
**
**--------------当前版本修订---------------------------------------------------
** 版  本: 
** 日　期: 2019年8月15日
** 描　述: 
		   
**-----------------------------------------------------------------------------
******************************************************************************/
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
#include "CT16.h"
#include "I2C.h"
#include "userApp.h"

DEVICE_PARAM Device_Param;


//计算零点周期的数据
uint8_t Int_Step = 1;

//按键处理数据
uint8_t  key_val = 0;
uint16_t key_time_power = 0;      //按键power时间计数
uint16_t key_time_dimup = 0;
uint16_t key_time_dimdown = 0;

//最低亮度设置
uint8_t KeyUpClick = 0;           //dim+长按单击标志
uint8_t KeyDownClick = 0;         //dim-长按单击标志
uint8_t workMode = MODE_NORMAL;   //0:正常工作模式 1:最低亮度设置模式
uint8_t SetFinishFlag = FALSE;    //设置完成标志 

uint8_t angleBackup = MIN_BR;     //保存设置最低亮度时的当前亮度值
uint32_t angleMinBackup = MIN_BR; //保存设置最低亮度时的当前最低亮度值
uint32_t brightnessBackup = 10; //保存设置最低亮度时的当前最低亮度百分比 %1

//最低亮度设置模式参数
uint16_t setTimeout;           //设置最低亮度模式超时

//串口处理数据
uint8_t UartRxStep = STEP0;        //串口接收步骤
uint8_t UartRxLen = 0;             //串口接收长度
uint8_t UartRxBuf[10];             //串口接收缓存
uint8_t ReceiveFinishFlag = FALSE; //串口接收完成标志
uint8_t UartRecvData1 = 0;
uint8_t UartRecvData2 = 0;
uint16_t TimeoutMs;
uint8_t IsSwitch;
uint8_t NormalSwitch = TRUE;

//零点调光数据
uint16_t frq_period = 24490;
uint16_t pwm_delay=MIN_BR;
uint32_t light_angle = MIN_BR;
uint32_t backup_light_angle = MIN_BR;
uint32_t brightness_adjust = MIN_BR;

//长按渐变处理数据
uint8_t dim_fade_add = FALSE;
uint8_t dim_fade_dec = FALSE;
uint8_t fade_add_count = 0;
uint8_t fade_dec_count = 0;
uint8_t report_count = 0;
uint8_t limit_report_count = 0;    //防止%1和%100多次上报
 
uint8_t Fre_Cnt_Flag = 0;
uint8_t Fre_Cnt = 0;

extern uint8_t PMU_Status;
extern volatile uint8_t FadeProcessFlag;
extern volatile uint8_t SetTimeoutFlag;
extern uint8_t I2C_ReceiveDataLen;
extern uint8_t I2C_ReceiveData[128];
extern uint8_t I2C_SendData[20];
extern uint8_t I2C_SendDataLen;

uint8_t frq_tab[6];
uint8_t frq_Cnt = 0;

uint8_t IIC_Recvflag = 0;
uint8_t IIC_SendFlag = 0;
uint8_t brightValue_bak = 0;

/*****************************************************************************
函数名称 : filter_data
功能描述 : 零点中断处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
uint16_t filter_data(void)
{
	uint8_t i,j,tmp;
	uint16_t sum=0;
	
	for(i=0; i<6-1; i++){
		for(j=0; j<6-1-i; j++){
			if(frq_tab[j]>frq_tab[j+1]){
				tmp = frq_tab[j+1];
				frq_tab[j+1] = frq_tab[j];
				frq_tab[j] = tmp;
			}
		}
	}
	
	for(i=1; i<5; i++){
		sum += frq_tab[i];
	}
	sum >>=2;
	
	return sum;
}

/*****************************************************************************
函数名称 : ZCD_Process
功能描述 : 零点中断处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
void ZCD_Process(void)
{	
	if(Fre_Cnt_Flag == TRUE)
	{
		//Fre_Cnt++;
		if(Fre_Cnt++ < 7)
		{
			if(Int_Step == 1)
			{
				Timer1_Value = 0;
				Int_Step = 2;
			}else if(Int_Step == 2)
			{
				Int_Step = 2;
				frq_tab[frq_Cnt++] = Timer1_Value;
				Timer1_Value = 0;
			}
		}else if(Fre_Cnt > 7)
		{
			Fre_Cnt = 0;
			frq_Cnt = 0;
			Fre_Cnt_Flag = FALSE;
			frq_period = filter_data();
			frq_period = frq_period*50*3;
		}
	}
//	  WDT_ReloadValue(250);
	//调光数据
	if(Device_Param.brightness_en == FALSE)
	{
			SN_CT16B2->PWMCTRL = mskCT16_PWM2EN_EN|mskCT16_PWM2MODE_FORCE_0|mskCT16_PWM2N_GPIO;
			SN_GPIO0->DATA_b.DATA3 = 0;
			return;
	}

	pwm_delay = (180-light_angle)*frq_period/180;

	SN_CT16B2->MCTRL = (mskCT16_MR9IE_EN|mskCT16_MR9STOP_EN);
	SN_CT16B2->PWMCTRL = mskCT16_PWM2EN_EN|mskCT16_PWM2MODE_2|mskCT16_PWM2N_MODE1;	//Enable PWM3N function, IO and select as PWM mode 2	
	SN_CT16B2->MR2 = pwm_delay;
	SN_CT16B2->MR9 = pwm_delay+2500;		
	SN_CT16B2->TC = 0;
	SN_CT16B2->TMRCTRL_b.CEN = 1;
	if(IIC_Recvflag == 1)
	{
		IIC_Recvflag = 0;
		IIC_SendFlag = 1;
	}
}

/*****************************************************************************
函数名称 : UartB_ReceiveData
功能描述 : 串口B接收数据
输入参数 : data : 接收的数据
返回参数 : 无
*****************************************************************************/
void UartB_ReceiveData(uint8_t RcvData)
{
	switch(UartRxStep)
	{
		case STEP0:
		{
			if(RcvData == FRAME_HEAD_1)
			{
				UartRxBuf[UartRxLen++] = RcvData;
				UartRxStep = STEP1;
				IsSwitch = FALSE;
				TimeoutMs = 0;
				NormalSwitch = FALSE;
			}
//			else
//			{
//					UartRxLen = 0;
//					UartRxStep = STEP0;
//			}
			break;
		}
		case STEP1:
		{
			UartRxBuf[UartRxLen++] = RcvData;
			if(UartRxLen > 2)
			{
				ReceiveFinishFlag = TRUE;
				UartRxStep = STEP0;
				UartRxLen = 0;
			}
		}
		break;
		default :
			break;
	}
}

/*****************************************************************************
函数名称 : Connect_With_Switch
功能描述 : 串口B开关互连处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
void Connect_With_Switch(void)
{
	uint8_t gpio_value = 0;
	
	//if(IsSwitch == TRUE && NormalSwitch == TRUE)
	{
		gpio_value = SN_GPIO3->DATA_b.DATA8;
		//gpio_value &= 0x01;
		if(backup_value != gpio_value) 
		{
			  backup_value = gpio_value;
				IsSwitch = FALSE;
				TimeoutMs = 0;
				if(Device_Param.led_switch == OFF)
				{
					Device_Param.led_switch = ON;
					Device_Param.brightness_en = TRUE;
					light_angle = Device_Param.led_angle;
				}
				else if(Device_Param.led_switch == ON)
				{
					Device_Param.led_switch = OFF;
					Device_Param.brightness_en = FALSE;
					Device_Param.led_angle = light_angle;
					light_angle = 0;
					Dimmer_Save_Prmtr();
				}
		}
	}
}

/*****************************************************************************
函数名称 : I2C_Write_Frame
功能描述 : I2C数据帧
输入参数 : 无
返回参数 : 无
*****************************************************************************/
void I2C_Write_Frame(const uint8_t cmd, uint8_t len, uint8_t value)
{
	unsigned char check_sum = 0;
  
  I2C_SendData[I2C_HEAD] = I2C_PROTOCOL_HEAD;
  I2C_SendData[I2C_LENGTH_HIGH] = 0;
  I2C_SendData[I2C_LENGTH_LOW] = len & 0xff;
  I2C_SendData[I2C_OP_CODE] = cmd;
  
  if(len > 1)
	{
			I2C_SendData[I2C_PARAMETER] = value;
	}
  len += 3;
  check_sum = I2C_CalcFCS((unsigned char *)I2C_SendData, len);
	I2C_SendData[len] = check_sum;
	I2C_SendDataLen = 0;
	//拉低I2C发送检测引脚
	SN_GPIO0->DATA_b.DATA8 = 0x00;
}

/*****************************************************************************
函数名称 : I2C_Soft_Reset
功能描述 : I2C 软件复位MCU
输入参数 : void
返回参数 : 无
*****************************************************************************/
static void I2C_Soft_Reset(void)
{
	system_factory_default();
}
/*****************************************************************************
函数名称 : I2C_Light_OnOff
功能描述 : I2C 开关处理
输入参数 : value：开/关值
返回参数 : 无
*****************************************************************************/
static void I2C_Light_OnOff(const uint8_t value)
{
		workMode = MODE_NORMAL;
		if(value == 0)
		{
				//开关关
				Device_Param.brightness_en = FALSE;
				
				if(workMode == MODE_BRIGHT_SET)
				{
						Device_Param.led_angle = workMode;
						Device_Param.led_angle_min = angleMinBackup;
						Device_Param.bright_value = brightnessBackup;
				}

				Device_Param.led_switch = OFF;
				light_angle = 0;
				PMU_Status = 0;
				UartB_SendData(UART_SWITCH, 0x00); //发送串口数据 
		}
		else if(value == 1)
		{
				//开关开
				Device_Param.led_switch = 1;
				Device_Param.brightness_en = TRUE;
				if(workMode == MODE_BRIGHT_SET)
				{
						Device_Param.led_angle = workMode;
						Device_Param.led_angle_min = angleMinBackup;
						Device_Param.bright_value = brightnessBackup;
						light_angle = Device_Param.led_angle;
				}
				else
				{	
						// 有调光值，显示对应亮度		
						if(Device_Param.led_angle)
						{
							light_angle = Device_Param.led_angle;
						}
				}
				UartB_SendData(UART_BRIGHT_VALUE, light_angle); //发送串口数据 
		}
		Device_Param.led_switch = value;
		I2C_Write_Frame(I2C_LIGHT_SWITCH_ACK,1, 0x00);
}

/*****************************************************************************
函数名称 : I2C_Light_Brightness
功能描述 : I2C亮度处理
输入参数 : value :亮度值
返回参数 : 无
*****************************************************************************/
void I2C_Light_Brightness(const uint8_t value)
{
		Device_Param.bright_value = value;
		Device_Param.led_angle = (unsigned char)(Device_Param.bright_value*(MAX_BR-Device_Param.led_angle_min)/100+Device_Param.led_angle_min);  // 40°导通角为%0  140°导通角为100%

		light_angle = Device_Param.led_angle;
		I2C_Write_Frame(I2C_LIGHT_BRIGHTNESS_ACK,2,value);
}

/*****************************************************************************
函数名称 : I2C_Get_Light_Status
功能描述 : I2C获取亮度数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void I2C_Get_Light_Status(void)
{
		uint8_t bright_value = 0;
	
		if(Device_Param.led_switch == OFF)
		{
				I2C_Write_Frame(I2C_GET_LIGHT_STATUS_ACK,2,0x00);
		}
		else 
		{
			  bright_value = (uint8_t)(Device_Param.bright_value);
				I2C_Write_Frame(I2C_GET_LIGHT_STATUS_ACK,2,bright_value);
		}
}

/*****************************************************************************
函数名称 : I2C_Get_Vession
功能描述 : I2C获取版本号
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void I2C_Get_Vession(void)
{
	unsigned char check_sum = 0;
  
  I2C_SendData[0] = I2C_PROTOCOL_HEAD;
  I2C_SendData[1] = 0;
  I2C_SendData[2] = 5;
  I2C_SendData[3] = I2C_GET_VERSION_ACK;
	I2C_SendData[4] = 20;
	I2C_SendData[5] = 4;
	I2C_SendData[6] = 24;
	I2C_SendData[7] = 1;

  check_sum = I2C_CalcFCS((unsigned char *)I2C_SendData, 8);
	I2C_SendData[9] = check_sum;
	I2C_SendDataLen = 0;
	//拉低I2C发送检测引脚
	SN_GPIO0->DATA_b.DATA8 = 0x00;
}

/*****************************************************************************
函数名称 : I2C_Process
功能描述 : I2C数据处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
void I2C_Process(void)
{
	uint8_t CheckSum = 0;
	uint8_t RecvLen = 0;
	uint8_t ReceData[20] = {0};
	
	memcpy(&ReceData,&I2C_ReceiveData,I2C_ReceiveDataLen);
	RecvLen = I2C_ReceiveDataLen;
	memset(I2C_ReceiveData, 0, I2C_ReceiveDataLen);
	I2C_ReceiveDataLen = 0;
	
	if(RecvLen < I2C_PROTOCOL_LEN)
		return;
	

	if(ReceData[I2C_HEAD] != I2C_PROTOCOL_HEAD)
		return;
	
	CheckSum = I2C_CalcFCS((unsigned char *)ReceData, ReceData[I2C_LENGTH_LOW]+3);
	
//	if(CheckSum != ReceData[RecvLen-1])
//		return;
	
	switch(ReceData[I2C_OP_CODE])
	{
		case I2C_SOFT_RESET:
			I2C_Soft_Reset();
			break;
		case I2C_LIGHT_SWITCH:
			I2C_Light_OnOff(ReceData[I2C_PARAMETER]);
			break;
		case I2C_LIGHT_BRIGHTNESS:
			brightValue_bak = ReceData[I2C_PARAMETER];
			Device_Param.bright_value = ReceData[I2C_PARAMETER];
			Device_Param.led_angle = (unsigned char)(Device_Param.bright_value*(MAX_BR-Device_Param.led_angle_min)/100+Device_Param.led_angle_min);  // 40°导通角为%0  140°导通角为100%

			light_angle = Device_Param.led_angle;
			//I2C_Light_Brightness(ReceData[I2C_PARAMETER]);
			IIC_Recvflag = 1;
			break;
		case I2C_GET_LIGHT_STATUS:
			I2C_Get_Light_Status();
			break;
		case I2C_GET_VERSION:
			I2C_Get_Vession();
			break;
		default:
			break;
	}
}

/*****************************************************************************
函数名称 : UartB_Write_Data
功能描述 : 串口1发送数据
输入参数 : data:发送的字符
返回参数 : 无
*****************************************************************************/
void UartB_Write_Data(uint8_t *uartData, uint8_t len)
{
	if((NULL == uartData) || (0 == len))
	{
		return ;
	}
	while(len--)
	{
		//发送
		UART1_SendByte(*uartData); 
		uartData++;
	}
	SN_UART1->CTRL =(UART_EN										//Enable UART0
									|	UART_RX_EN									//Enable RX
									| UART_TX_EN);								//Enable TX
	//UT_DelayNms(10);
  GPIO_Clr(GPIO_PORT2, GPIO_PIN2);
}

/*****************************************************************************
函数名称 : UartB_SendData
功能描述 : 串口B发送数据
输入参数 : cmd : 命令 data : 发送的同步数据
返回参数 : 无
*****************************************************************************/
void UartB_SendData(uint8_t cmd, uint8_t sync_data)
{
	uint8_t TxBuf[3];
	
	//发送时关闭串口接收中断
	SN_UART1->CTRL &= 0xBF;
	GPIO_Set (GPIO_PORT2, GPIO_PIN2);
	UT_DelayNms(2);

	TxBuf[0] = FRAME_HEAD_1;
	TxBuf[1] = cmd;
	TxBuf[2] = sync_data;
	
	UartB_Write_Data((uint8_t *)TxBuf, 3);
}

/*****************************************************************************
函数名称 : Key_Scan
功能描述 : 按键扫描
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static uint8_t Key_Scan(void)
{
	uint8_t temp=0;
	if((SN_GPIO2->DATA_b.DATA6 ) == 0) temp|=0x01;  // P0.8 Power
	if((SN_GPIO1->DATA_b.DATA0 ) == 0) temp|=0x02;  // P3.9 DIM+
	if((SN_GPIO2->DATA_b.DATA7 ) == 0) temp|=0x04;  // P0.5 DIM-
	return temp;
}

/*****************************************************************************
函数名称 : Key_Check
功能描述 : 按键检测
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static uint8_t Key_Check(void)
{
	static uint8_t keyPower = FALSE;
	static uint8_t keyDimUp = FALSE;
	static uint8_t keyDimDown = FALSE;
	
	static uint8_t keyPower_Val = KEY_NULL;
	static uint8_t keyDimUp_Val = KEY_NULL;
	static uint8_t keyDimDown_Val = KEY_NULL;

	uint8_t keyTemp_Val = KEY_NULL;
	
	key_val = Key_Scan();
	
	//key_power  check process*******************************************// 
	if(key_val&KEY_POWER){
		if(!keyPower){
			keyPower = TRUE;
			key_time_power = ZERO;
		}
	}else{
		keyPower = FALSE;
	}
	
	if(keyPower){
		if(key_time_power>KEY_TIMEZERO && key_time_power<=KEY_TIMESHORT&&keyPower_Val==KEY_NULL){
			keyPower_Val = KEY_SHORT_DOWN;
			keyTemp_Val = keyPower_Val;
		}else if(key_time_power>KEY_TIMELONG_A&&keyPower_Val==KEY_SHORT_DOWN){
			keyPower_Val = KEY_LONG_DOWN;
			keyTemp_Val = keyPower_Val;
		}
	}else{
		switch(keyPower_Val){
			case KEY_SHORT_DOWN:
				keyPower_Val = KEY_SHORT_UP;
				keyTemp_Val = keyPower_Val;
				keyPower_Val = KEY_NULL;
				break;
				
			case KEY_LONG_DOWN:
				keyPower_Val = KEY_LONG_UP;
				keyTemp_Val = keyPower_Val;
				keyPower_Val = KEY_NULL;
				break;
		}
	}
	
	if(Device_Param.brightness_en  == TRUE)
	{
		//key_up  check process **********************************************// 
		if(key_val&KEY_DIM_UP){
			if(!keyDimUp){
				keyDimUp = TRUE;
				key_time_dimup = ZERO;
			}
		}else{
			keyDimUp = FALSE;
		}
		
		if(keyDimUp){
			if(key_time_dimup>KEY_TIMEZERO&&key_time_dimup<KEY_TIMESHORT&&keyDimUp_Val==KEY_NULL){
				keyDimUp_Val = KEY_ADD_SHORT_DOWN;
				keyTemp_Val = keyDimUp_Val;
				//KeyUpClick = 0;
			}else if(key_time_dimup>1000 && key_time_dimup<KEY_TIMELONG){
				if((key_val&KEY_DIM_DOWN) != KEY_DIM_DOWN)
				{
					KeyUpClick = 1;
				}else if((key_val&KEY_DIM_DOWN) == KEY_DIM_DOWN){
					//按键组合
					KeyUpClick = 2;
				}
			}else if((key_time_dimup>KEY_TIMELONG) && (KeyUpClick == 1)){
				keyDimUp_Val = KEY_ADD_LONG_DOWN;
				keyTemp_Val = keyDimUp_Val;
			}else if((key_time_dimup>KEY_TIMELONG_A) && (KeyUpClick == 2)){
				keyDimUp_Val = KEY_ADD_LONG_5S_DOWN;
				keyTemp_Val = keyDimUp_Val;
				KeyUpClick = 3;
			}
			
		}else{
			switch(keyDimUp_Val){
				case KEY_ADD_SHORT_DOWN:
					keyDimUp_Val = KEY_ADD_SHORT_UP;
					keyTemp_Val = keyDimUp_Val;
					keyDimUp_Val = KEY_NULL;
					break;
				case KEY_ADD_LONG_DOWN:
					keyDimUp_Val = KEY_ADD_LONG_UP;
					keyTemp_Val = keyDimUp_Val;
					keyDimUp_Val = KEY_NULL;
					break;
				case KEY_ADD_LONG_5S_DOWN:
					//keyDimUp_Val = KEY_ADD_LONG_UP;
					keyTemp_Val = keyDimUp_Val;
					keyDimUp_Val = KEY_NULL;
					break;
			}
		}
	}

    if(Device_Param.brightness_en  == TRUE)
	{
		//key_dim_down  check process **********************************************// 
		if(key_val&KEY_DIM_DOWN){
			if(!keyDimDown){
				keyDimDown = TRUE;
				key_time_dimdown = ZERO;
			}
		}else{
			keyDimDown = FALSE;
		}
		
		if(keyDimDown){
			if(key_time_dimdown>KEY_TIMEZERO&&key_time_dimdown<KEY_TIMESHORT&&keyDimDown_Val==KEY_NULL){ 
				keyDimDown_Val = KEY_DEC_SHORT_DOWN;
				keyTemp_Val = keyDimDown_Val;
				//KeyDownClick = 0;
			}else if(key_time_dimdown>1000 && key_time_dimdown<KEY_TIMELONG){
				if((key_val&KEY_DIM_UP) != KEY_DIM_UP)
				{
					KeyDownClick = 1;
				}else if((key_val&KEY_DIM_UP) == KEY_DIM_UP){
					//按键组合
					KeyDownClick = 2;
				}
			}else if(key_time_dimdown>KEY_TIMELONG && (KeyDownClick == 1)){
				keyDimDown_Val = KEY_DEC_LONG_DOWN;
				keyTemp_Val = keyDimDown_Val;
			}else if((key_time_dimdown>KEY_TIMELONG_A) && (KeyDownClick == 2)){
				keyDimDown_Val = KEY_DEC_LONG_5S_DOWN;
				keyTemp_Val = keyDimUp_Val;
				KeyDownClick = 3;
			}
			
		}else{
			switch(keyDimDown_Val){
				case KEY_DEC_SHORT_DOWN:
					keyDimDown_Val = KEY_DEC_SHORT_UP;
					keyTemp_Val = keyDimDown_Val;
					keyDimDown_Val = KEY_NULL;
					break;
				
				case KEY_DEC_LONG_DOWN:
					keyDimDown_Val = KEY_DEC_LONG_UP;
					keyTemp_Val = keyDimDown_Val;
					keyDimDown_Val = KEY_NULL;
					break;
				case KEY_DEC_LONG_5S_DOWN:
					//keyDimDown_Val = KEY_DEC_LONG_UP;
					keyTemp_Val = keyDimDown_Val;
					keyDimDown_Val = KEY_NULL;
					break;		
			}
		}
	}
	return keyTemp_Val;
}

/*****************************************************************************
函数名称 : Key_Power_ShortPress_Handle
功能描述 : power按键短按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Power_ShortPress_Handle(void)
{
		if(Device_Param.led_switch == OFF) 
		{
				Device_Param.led_switch = ON;
				Device_Param.brightness_en = TRUE;
				if(workMode == MODE_BRIGHT_SET)
				{
						Device_Param.led_angle = angleBackup;
						Device_Param.led_angle_min = angleMinBackup;
						Device_Param.bright_value = brightnessBackup;
						light_angle = Device_Param.led_angle;
				}
				else
				{
						//DIM_H;
						if(Device_Param.flag == FALSE)
						{
							DIM_H;
							Device_Param.led_angle = MAX_BR;
							light_angle = Device_Param.led_angle;
						}
						else
						{
							light_angle = Device_Param.led_angle;
						}
						//UartB_SendData(UART_BRIGHT_VALUE_MIN, Device_Param.led_angle_min); //开灯同步最低亮度
				}
				UartB_SendData(UART_BRIGHT_VALUE, light_angle); //发送串口数据
				I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,20);
		}
		else
		{
				Device_Param.led_switch = OFF;
				//UartB_SendData(UART_SWITCH, 0x00); //发送串口数据 
				
				Device_Param.brightness_en = FALSE;
					//DIM_L;
				Device_Param.led_angle = light_angle;
				light_angle = 0;
				PMU_Status = 0;

				{
					UartB_SendData(UART_SWITCH, 0x00); //发送串口数据 
					I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,0);
				}
				Dimmer_Save_Prmtr();
		}
}

/*****************************************************************************
函数名称 : Key_Power_LongPress_Handle
功能描述 : power按键长按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Power_LongPress_Handle(void)
{
		if(workMode == MODE_BRIGHT_SET)
		{
				Device_Param.led_angle = angleBackup;
				Device_Param.led_angle_min = angleMinBackup;
				Device_Param.bright_value = brightnessBackup;
				light_angle = Device_Param.led_angle;
		}
		
		// 恢复出厂设置
		system_factory_default();
}

/*****************************************************************************
函数名称 : Key_Add_ShortPress_Handle
功能描述 : dimmer+按键短按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Add_ShortPress_Handle(void)
{
		int32_t bright_value = 0;
		uint8_t percent = 0;
		
		if(Device_Param.brightness_en == TRUE)
		{
				Device_Param.led_switch = ON;
				(workMode == MODE_NORMAL) ? (SetFinishFlag = FALSE):(SetFinishFlag = TRUE);
				//正常工作模式
				if(workMode == MODE_NORMAL)
				{
						Device_Param.led_angle = light_angle;
						brightness_adjust = Device_Param.led_angle;
							
						//当前导通角度值
						bright_value = ((brightness_adjust-Device_Param.led_angle_min)*100/(MAX_BR-Device_Param.led_angle_min));

						if((bright_value >= 0) && (bright_value < 5))
						{
							bright_value = 5;
							percent = 5;
						}else if((bright_value >= 5) && (bright_value < 20))
						{
								bright_value = 20;
								percent = 20;
						}else if((bright_value >= 20) && (bright_value < 35))
						{
								bright_value = 35;
								percent = 35;
						}else if((bright_value >= 35) && (bright_value < 50))
						{
								bright_value = 50;
								percent = 50;
						}else if((bright_value >= 50) && (bright_value < 65))
						{
								bright_value = 65;
								percent = 65;
						}else if((bright_value >= 65) && (bright_value < 80))
						{
								bright_value = 80;
								percent = 80;
						}else if((bright_value >= 80) && (bright_value <= 100))
						{
								bright_value = 100;
								percent = 100;
						}
						light_angle = percent*(MAX_BR-Device_Param.led_angle_min)/100+Device_Param.led_angle_min;
							
						Device_Param.led_switch = ON;	
						Device_Param.led_angle =  light_angle;
						Device_Param.bright_value = bright_value;

						Device_Param.flag = TRUE;
						{
								UartB_SendData(UART_BRIGHT_KEY_PRESS, bright_value); //发送串口数据 
								I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,bright_value);
						}
				}
				else if(workMode == MODE_BRIGHT_SET)
				{
					brightness_adjust = Device_Param.led_angle;

					if(brightness_adjust < MAX_BR)
					{
						brightness_adjust += 1;
					}
					else
					{
						// 最大亮度
						brightness_adjust = MAX_BR;
					}
					light_angle = brightness_adjust;
			
					Device_Param.led_angle =  light_angle;
					Device_Param.bright_value = (light_angle-Device_Param.led_angle_min)*100/(MAX_BR-Device_Param.led_angle_min);
					Device_Param.flag = TRUE;
					{
						UartB_SendData(UART_BRIGHT_VALUE, Device_Param.led_angle); //发送串口数据	
						I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,Device_Param.bright_value);
					}				
				}
		}
}

/*****************************************************************************
函数名称 : Key_Add_LongPress_Down_Handle
功能描述 : dimmer+按键长按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Add_LongPress_Down_Handle(void)
{
		(workMode == MODE_NORMAL) ? (SetFinishFlag = FALSE):(SetFinishFlag = TRUE);
		dim_fade_add = TRUE;
		Device_Param.led_switch = ON;
		if(report_count % 4 == 0)
		{
				report_count = 0;
				Device_Param.led_angle =  light_angle;
				if(Device_Param.led_angle == Device_Param.led_angle_min)
				{
						Device_Param.bright_value = 10;
				}
				else
				{
						Device_Param.bright_value = (light_angle-Device_Param.led_angle_min)*100/(MAX_BR -Device_Param.led_angle_min);
				}
				if(Device_Param.bright_value >= 100)
				{
						Device_Param.bright_value = 100;
				}

				if(Device_Param.led_angle == 140)
				{
						limit_report_count++;
						if(limit_report_count >= 2)
						{
							return;
						}
				}
				UartB_SendData(UART_BRIGHT_VALUE, Device_Param.led_angle); //发送串口数据		
				I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,Device_Param.bright_value);
		}
}

/*****************************************************************************
函数名称 : Key_Add_LongPress_Up_Handle
功能描述 : dimmer+按键长按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Add_LongPress_Up_Handle(void)
{
		dim_fade_add = FALSE;
		limit_report_count = 0;
		KeyUpClick = 0;
}

/*****************************************************************************
函数名称 : Key_Dec_ShortPress_Handle
功能描述 : dimmer-按键短按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Dec_ShortPress_Handle(void)
{
		int32_t bright_value = 0;
		uint8_t percent = 0;
		
		if(Device_Param.brightness_en == TRUE)
		{
				(workMode == MODE_NORMAL) ? (SetFinishFlag = FALSE):(SetFinishFlag = TRUE);
				if(workMode == MODE_NORMAL)//正常工作模式
				{
						Device_Param.led_angle = light_angle;
						brightness_adjust = Device_Param.led_angle;
						//backup_light_angle = light_angle;
					
						//当前导通角度值
						bright_value = ((brightness_adjust-Device_Param.led_angle_min)*100/(MAX_BR-Device_Param.led_angle_min));
						if((bright_value >= 0) && (bright_value <= 8))
						{
							bright_value = 1;
							percent = 1;
						}else if((bright_value > 8) && (bright_value <= 25))
						{
							bright_value = 5;
							percent = 5;
						}else if((bright_value > 25) && (bright_value <= 40))
						{
							bright_value = 20;
							percent = 20;
						}else if((bright_value > 40) && (bright_value <= 55))
						{
							bright_value = 35;
							percent = 35;
						}else if((bright_value > 55) && (bright_value <= 70))
						{
							bright_value = 50;
							percent = 50;
						}else if((bright_value > 70) && (bright_value <= 85))
						{
							bright_value = 65;
							percent = 65;
						}else if((bright_value > 85) && (bright_value <= 100))
						{
							bright_value = 80;
							percent = 80;
						}
						light_angle = percent*(MAX_BR-Device_Param.led_angle_min)/100+Device_Param.led_angle_min;
						//backup_light_angle = percent*(MAX_BR-Device_Param.led_angle_min)/100+Device_Param.led_angle_min;
						
						Device_Param.led_switch = ON;
						Device_Param.led_angle =  light_angle;			
						Device_Param.bright_value = bright_value;
						
						Device_Param.flag = TRUE; 

						{		 
							UartB_SendData(UART_BRIGHT_KEY_PRESS, percent); //发送串口数据
							I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,bright_value);
						}
				}
				else if(workMode == MODE_BRIGHT_SET)
				{
						brightness_adjust = Device_Param.led_angle;
						
						if(brightness_adjust > MIN_BR)
						{
								brightness_adjust -= 1;
						}
						else
						{
							// 最低亮度
							brightness_adjust = MIN_BR;
						}
						light_angle = brightness_adjust;
						Device_Param.led_angle = light_angle;
						
						if(Device_Param.led_angle == Device_Param.led_angle_min)
						{
							Device_Param.bright_value = 10;
						} 
						else
						{
							Device_Param.bright_value = (light_angle-Device_Param.led_angle_min)*1000/(MAX_BR-Device_Param.led_angle_min);
						}

						Device_Param.flag = TRUE; 
						UartB_SendData(UART_BRIGHT_VALUE, Device_Param.led_angle); //发送串口数据 
						I2C_Write_Frame(I2C_UPDATE_LIGHT_SET,2,Device_Param.bright_value/10);
				}
		}
}

/*****************************************************************************
函数名称 : Key_Dec_LongPress_Down_Handle
功能描述 : dimmer-按键长按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Dec_LongPress_Down_Handle(void)
{
		(workMode == MODE_NORMAL) ? (SetFinishFlag = FALSE):(SetFinishFlag = TRUE);
		dim_fade_dec = TRUE;
		
		Device_Param.led_switch = ON;
		if(report_count % 4 == 0)
		{
				report_count = 0;
				Device_Param.led_angle =  light_angle;
				if(Device_Param.led_angle == Device_Param.led_angle_min)
				{
					Device_Param.bright_value = 10;
				}
				else
				{
					Device_Param.bright_value = (light_angle-Device_Param.led_angle_min)*100/(MAX_BR-Device_Param.led_angle_min);
				}
				if(Device_Param.led_angle == Device_Param.led_angle_min)
				{
					limit_report_count++;
					if(limit_report_count >= 2)
						return;
				} 
				UartB_SendData(UART_BRIGHT_VALUE, Device_Param.led_angle); //发送串口数据 
				I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,Device_Param.bright_value);
		}
}

/*****************************************************************************
函数名称 : Key_Dec_LongPress_Up_Handle
功能描述 : dimmer-按键长按处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void Key_Dec_LongPress_Up_Handle(void)
{
		dim_fade_dec = FALSE;
		limit_report_count = 0;
		KeyDownClick = 0;
}

/*****************************************************************************
函数名称 : Key_Process
功能描述 : 按键处理
输入参数 : 无
返回参数 : 无
*****************************************************************************/
void Key_Process(void)
{
    uint8_t Key_Value = KEY_NULL;
	
	  Key_Value = Key_Check();
	
	  switch(Key_Value)
		{
			// 电源键短按抬起
			case KEY_SHORT_UP:
			{
				  workMode = MODE_NORMAL;
				  SetFinishFlag = FALSE;
				  Key_Power_ShortPress_Handle();
					
					break;
			}
			// 电源键长按未抬起
			case KEY_LONG_DOWN:
			{
				  workMode = MODE_NORMAL;
				  SetFinishFlag = FALSE;
				  Key_Power_LongPress_Handle();
					break;
			}
			// dim+ 短按抬起
			case KEY_ADD_SHORT_UP:
			{
					Key_Add_ShortPress_Handle();
					break;
			}
			// dim+ 长按未抬起
			case KEY_ADD_LONG_DOWN:
			{
					Key_Add_LongPress_Down_Handle();
					break;
			}
			case KEY_ADD_LONG_UP:
			{
					Key_Add_LongPress_Up_Handle();
					break;
			}
			// dim- 短按抬起
			case KEY_DEC_SHORT_UP:
			{
					Key_Dec_ShortPress_Handle();
					break;
			}
			// dim- 长按未抬起
			case KEY_DEC_LONG_DOWN:
			{
				  Key_Dec_LongPress_Down_Handle();
				  break;
			}
			// dim- 长按抬起
			case KEY_DEC_LONG_UP:
			{
				  Key_Dec_LongPress_Up_Handle();
				  break;
			}
			default:
					break;
		}
	
		if((KeyUpClick == 3) && (KeyDownClick == 3))
		{
		    //进入最低亮度设置模式
			  KeyUpClick = 0;
				KeyDownClick = 0;
				key_time_dimup = 0;
				key_time_power = 0;
				key_time_dimdown = 0;

				if(SetFinishFlag == FALSE)
				{
				    workMode = MODE_BRIGHT_SET;
						setTimeout = 0;
						SetTimeoutFlag = FALSE;
						//保存当前值
						angleBackup = light_angle;
						angleMinBackup = Device_Param.led_angle_min;
						brightnessBackup = Device_Param.bright_value;
						Device_Param.led_angle_min = 40;
						Device_Param.led_angle = 40;
						Device_Param.bright_value = 10;
						light_angle = Device_Param.led_angle;
			
						UartB_SendData(UART_BRIGHT_VALUE, light_angle); //发送串口数据 
						I2C_Write_Frame(I2C_UPDATE_LIGHT_SET,2,20);
				}
				else
				{
						//设置完成
						workMode = MODE_NORMAL;
						Device_Param.led_angle = light_angle;
						Device_Param.led_angle_min = Device_Param.led_angle;
						Device_Param.bright_value = 10;
						SetTimeoutFlag = FALSE;
					
						UartB_SendData(UART_BRIGHT_VALUE_MIN, Device_Param.led_angle_min); //设置完，同步最低亮度
					  I2C_Write_Frame(I2C_UPDATE_LIGHT_SET_COMPLETE,2,20);
				}
		}
}

/**************************************************************************** 
 * 函数名称 : Fade_Process
 * 功能描述 : 处理渐变数据
 * 参    数 : 无
 * 参    数 : 无
 * 返 回 值 : 无
 *****************************************************************************/
void Fade_Process(void)
{
	//渐变处理
		if(FadeProcessFlag == TRUE)
		{
			FadeProcessFlag = FALSE;
			if(dim_fade_add == TRUE)
			{
				fade_add_count++;
				report_count++;
				if(fade_add_count % 4 == 0)
				{
					fade_add_count = 0;
					if(light_angle < MAX_BR)
					{
						light_angle++;
						Device_Param.led_angle = light_angle;
					}
				}
			}
			
			if(dim_fade_dec == TRUE)
			{
				fade_dec_count++;
				report_count++;
				if(fade_dec_count % 4 == 0)
				{
					fade_dec_count = 0;
					if(light_angle > Device_Param.led_angle_min)
					{
						light_angle--;
						Device_Param.led_angle = light_angle;
					}
				}
			}
		}
}

/**************************************************************************** 
 * 函数名称 : SetTimeout_Process
 * 功能描述 : 处理设置最低亮度超时事件
 * 参    数 : 无
 * 参    数 : 无
 * 返 回 值 : 无
 *****************************************************************************/
void SetTimeout_Process(void)
{
	if(workMode == MODE_BRIGHT_SET)
	{
		if(SetTimeoutFlag == TRUE)
		{
			SetTimeoutFlag = FALSE;
			workMode = MODE_NORMAL;
			SetFinishFlag = FALSE;
			Device_Param.led_angle = angleBackup;
			Device_Param.led_angle_min = angleMinBackup;
			Device_Param.bright_value = brightnessBackup;
			light_angle = Device_Param.led_angle;
			Device_Param.bright_value = ((light_angle-Device_Param.led_angle_min)*1000/(MAX_BR-Device_Param.led_angle_min));
		 
			UartB_SendData(UART_BRIGHT_VALUE, Device_Param.led_angle); //?????? 
			UartB_SendData(UART_BRIGHT_VALUE_MIN, Device_Param.led_angle_min); //?????? 
			I2C_Write_Frame(I2C_UPDATE_LIGHT_SET,2,(uint8_t)Device_Param.bright_value);
		}
	}
}
/**************************************************************************** 
 * 函数名称 : UartB_Process
 * 功能描述 : 处理串口数据
 * 参    数 : 无
 * 参    数 : 无
 * 返 回 值 : 无
 *****************************************************************************/
void UartB_Process(void)
{
	uint32_t bright_value = 0;
	
	if(ReceiveFinishFlag == FALSE)
		return;
	
	ReceiveFinishFlag = FALSE;
	
	UartRecvData1 = UartRxBuf[1];
  UartRecvData2 = UartRxBuf[2];
	
	switch(UartRecvData1)
	{
		//开关同步
		case UART_SWITCH:
		{
			Device_Param.brightness_en = FALSE;
		  Device_Param.led_switch = OFF;
			//DIM_L;
		  Device_Param.led_angle = light_angle;
			light_angle = 0;

			Dimmer_Save_Prmtr();
			break;
		}
		//亮度同步
		case UART_BRIGHT_VALUE:
		{
			Device_Param.led_switch = ON;
			Device_Param.brightness_en = TRUE;
			Device_Param.led_angle = UartRecvData2;
			light_angle = Device_Param.led_angle; 

			if(Device_Param.led_angle == Device_Param.led_angle_min)
			{
				Device_Param.bright_value = 10;
			}
			else
			{
				Device_Param.bright_value = (light_angle-Device_Param.led_angle_min)*1000/(MAX_BR-Device_Param.led_angle_min);
			}

			break;
		}
		//最低亮度同步
		case UART_BRIGHT_VALUE_MIN:
		{
			Device_Param.led_angle_min = UartRxBuf[2];
			break;
		}
		//按键亮度同步
		case UART_BRIGHT_KEY_PRESS:
		{	
			Device_Param.led_switch = ON;
			Device_Param.brightness_en = TRUE;

			Device_Param.led_angle = (MAX_BR - Device_Param.led_angle_min)*UartRecvData2/100 + Device_Param.led_angle_min;
			light_angle = Device_Param.led_angle; 
			bright_value = UartRecvData2*10;
			
			Device_Param.bright_value = bright_value;
			break;
		}
		default:
			break;
	}
}

/**************************************************************************** 
 * 函数名称 : Led_Process
 * 功能描述 : 处理指示灯数据
 * 参    数 : 无
 * 参    数 : 无
 * 返 回 值 : 无
 *****************************************************************************/
void Led_Process(void)
{
	if(Device_Param.led_switch == ON )
	{
		//SN_GPIO2->DATA_b.DATA1 = 0x1;		// enable ZCD
	}
	else if(Device_Param.led_switch == OFF)
	{
		//SN_GPIO2->DATA_b.DATA1 = 0x0;		// disable ZCD
	}
}

/*******************************************************************************
* 函数名称  : I2C_CalcFCS
* 功能描述  : I2C消息校验函数
* 参　　数  : const uint8 *msg_ptr
* 参　　数  : uint8 len
* 返 回 值  : uint8_t
* 作　　者  :
* 设计日期  : 2014年8月4日
* 修改日期         修改人         修改内容
*******************************************************************************/
uint8_t I2C_CalcFCS( const uint8_t *msg_ptr, uint8_t len )
{
    uint8_t x;
    uint8_t xorResult;
    
    xorResult = 0;
    
    for ( x = 0; x < len; x++, msg_ptr++ )
      xorResult = xorResult ^ *msg_ptr;
    
    return ( xorResult );
}

/**************************************************************************** 
 * 函数名称 : Dimmer_Save_Prmtr
 * 功能描述 : 保存数据
 * 参    数 : 无
 * 参    数 : 无
 * 返 回 值 : 无
 *****************************************************************************/
void Dimmer_Save_Prmtr(void)
{
	Device_Param.flag = 0xAA;
	Device_Param.led_type = 0x01;
	//Device_Param.led_switch = OFF;
	
	if (FLASH_ProgramPage(ISP_TARGET_ADDR, sizeof(Device_Param), (uint8_t *)(&Device_Param)) == FAIL){
				while(1);			//Program Fail
	}
	UT_DelayNx10us(20);
}

/*****************************************************************************
函数名称 : system_factory_default
功能描述 : 恢复出厂设置
输入参数 : 无
返回参数 : 无
*****************************************************************************/
static void system_factory_default(void)
{
	Device_Param.flag = 0xAA;
	Device_Param.led_type = 0x01;
	//Device_Param.led_switch = OFF;
	Device_Param.led_angle = MAX_BR;
	Device_Param.led_angle_min = MIN_BR;
	Device_Param.bright_value = 1000;
	Device_Param.brightness_min = 10;
  light_angle = Device_Param.led_angle;

	{
		UartB_SendData(UART_BRIGHT_VALUE, Device_Param.led_angle); 
		I2C_Write_Frame(I2C_UPDATE_LIGHT_STATUS,2,20);
	}
	if (FLASH_ProgramPage(ISP_TARGET_ADDR, sizeof(Device_Param), (uint8_t *)(&Device_Param)) == FAIL){
				while(1);			//Program Fail
	}
}

