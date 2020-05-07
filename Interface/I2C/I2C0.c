/******************** (C) COPYRIGHT 2015 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2015/08
* AUTHOR:				SA1
* IC:						SN32F700B/710B
* DESCRIPTION:	I2C0 related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2015/08/14	SA1			1. First release
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
#include "I2C.h"
#include "userApp.h"
#include "Utility.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
//------------------I2C-------------------------
////Address
//uint16_t hwI2C_Device_Addr_I2C0 = 0x00;
//uint16_t hwI2C_Device_Addr_I2C1 = 0x00;

//Check Flag
uint32_t wI2C_TimeoutFlag = 0;
uint32_t wI2C_ArbitrationFlag = 0;

//Error Flag
uint32_t wI2C_RegisterCheckError = 0;
uint32_t wI2C_TotalError = 0;

uint8_t I2C_Reg;
uint8_t I2C_ReceiveDataLen = 0;
uint8_t I2C_ReceiveData[128];
uint8_t I2C_SendDataLen = 0;
uint8_t I2C_SendData[20];
uint8_t wI2C_ReceiveFinishFlag = 0;

enum{
  GET_REG_STATE,
  GET_DATA_STATE,
  SEND_DATA_STATE,
  IDLE_STATE,
};

uint8_t I2C_State = IDLE_STATE;

extern uint8_t IIC_HeatTimer;

/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/
/*****************************************************************************
* Function		: I2C_swSTOP
* Description	: 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void I2C_swSTOP(void)
{
	SN_I2C0->STAT_b.I2CIF = 1;		
	SN_I2C0->CTRL_b.I2CEN = 0;			//Disable I2C		
	SN_GPIO0->MODE_b.MODE4 = 1;
	SN_GPIO0->MODE_b.MODE5 = 1;
	SN_GPIO0->BCLR = 1 << 5;
	SN_GPIO0->BSET = 1 << 4;
	
	UT_DelayNx10us(50);
	SN_GPIO0->BSET = 1 << 5;
  SN_I2C0->CTRL_b.I2CEN = 1;

}
/*****************************************************************************
* Function		: I2C0_Init
* Description	: Set specified value to specified bits of assigned register
* Input			: wI2C0SCLH - SCL High Time
*				  		wI2C0SCLL - SCL Low Time 
*				  		wI2C0Mode - 0: Standard/Fast mode.1: Fast-mode Plus 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void I2C0_Init(void)
{
	//I2C0 interrupt enable
	NVIC_ClearPendingIRQ(I2C0_IRQn);	
	NVIC_EnableIRQ(I2C0_IRQn);
	NVIC_SetPriority(I2C0_IRQn,0);

	//Enable HCLK for I2C0
	SN_SYS1->AHBCLKEN_b.I2C0CLKEN = 1;						//Enable clock for I2C0

//    //³¬Ê±¼ì²â
	SN_I2C0->TOCTRL = 128;
	//I2C speed
//	SN_I2C0->SCLHT = I2C0_SCLHT;
//	SN_I2C0->SCLLT = I2C0_SCLLT;
	Set_I2C0_Address(0,0,0x52,0);
	//I2C enable
	SN_I2C0->CTRL_b.I2CEN = I2C_I2CEN_EN;

}

/*****************************************************************************
* Function		: I2C0_Timeout_Ctrl
* Description	: Set specified value to specified bits of assigned register
* Input			: wI2CTo - TimeOut Value: wI2CTo * 32 * I2C_PCLK cycle
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void I2C0_Timeout_Ctrl(uint32_t wI2CTo) 
{
		SN_I2C0->TOCTRL = wI2CTo;
}

/*****************************************************************************
* Function		: Set_I2C0_Address
* Description	: Set specified value to specified bits of assigned register
* Input			: bI2CaddMode - 7 bits address is 0, 10 bits address is 1 
*				  		bSlaveNo - Slave address number 0, 1, 2, 3
*				  		bSlaveAddr - Slave value
*				  		bGCEnable - Genral call enable is 1
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void Set_I2C0_Address(uint8_t bI2CaddMode, uint8_t bSlaveNo, uint32_t bSlaveAddr, uint8_t bGCEnable)
{
	volatile uint16_t hwAddressCom=0;  


	if(bI2CaddMode == 0)
	{	
		hwAddressCom = bSlaveAddr << 1;
	}
	else
	{
		hwAddressCom = bSlaveAddr;
	}

	if(bGCEnable == 1)
	{
		SN_I2C0->SLVADDR0_b.GCEN = I2C_GCEN_EN;
	}
	else
	{
		SN_I2C0->SLVADDR0_b.GCEN = I2C_GCEN_DIS;
	}

	if(bI2CaddMode == 1)
	{
		SN_I2C0->SLVADDR0_b.ADD_MODE = I2C_ADD_MODE_10BIT;
	}
	else
	{
		SN_I2C0->SLVADDR0_b.ADD_MODE = I2C_ADD_MODE_7BIT;
	}
			
	switch (bSlaveNo)
	{
		case 0:
		SN_I2C0->SLVADDR0 = hwAddressCom;	
		break;
			
		case 1:
		SN_I2C0->SLVADDR1 = hwAddressCom;	
		break;
			
		case 2:
		SN_I2C0->SLVADDR2 = hwAddressCom;	
		break;
			
		case 3:
		SN_I2C0->SLVADDR3 = hwAddressCom;	
		break;
		  
		default:
		break;
	}
}

/*****************************************************************************
* Function		: I2C0_Enable
* Description	: I2C0 enable setting
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void I2C0_Enable(void)
{
	//Enable HCLK for I2C0
	SN_SYS1->AHBCLKEN |= (0x1 << 21);								//Enable clock for I2C0

	SN_I2C0->CTRL_b.I2CEN = I2C_I2CEN_EN;						//I2C enable
}

/*****************************************************************************
* Function		: I2C0_Disable
* Description	: I2C0 disable setting
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void I2C0_Disable(void)
{
	SN_I2C0->CTRL_b.I2CEN = I2C_I2CEN_DIS;					//I2C disable
	
	//Disable HCLK for I2C0
	SN_SYS1->AHBCLKEN &=~ (0x1 << 21);							//Disable clock for I2C0
}

/*****************************************************************************
* Function		: I2C0_Slave_Tx
* Description	: Set specified value to specified bits of assigned register
* Input			: *bDataFIFO - Declare TX FIFO Register  
*				  		*bPointerFIFO - Declare TX FIFO Pointer Register
*				  		*bCommStop - Declare the Register when get the STOP information
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void I2C0_Slave_IRQ(void)
{
	if(SN_I2C0->STAT & mskI2C_TIMEOUT_TIMEOUT) 					//Timeout State		
	{
		SN_I2C0->STAT |= mskI2C_I2CIF_INTERRUPT;					//Clear I2C flag
		I2C_swSTOP();
	}
	else if(SN_I2C0->STAT & mskI2C_LOST_ARB_LOST_ARBITRATION)	//ARB State	
	{
		SN_I2C0->STAT |= mskI2C_I2CIF_INTERRUPT;					//Clear I2C flag
		I2C_swSTOP();
	}
	else if( SN_I2C0->STAT & mskI2C_STOP_DN_STOP)				//Stop Down
	{
		//receive complete
		SN_I2C0->STAT |= mskI2C_I2CIF_INTERRUPT;					//Clear I2C flag
		//wI2C_ReceiveFinishFlag = 1;
		I2C_Process();
	}
	else
	{	
		SN_I2C0->STAT |= mskI2C_I2CIF_INTERRUPT;					//Clear I2C flag

		switch (SN_I2C0->STAT)
		{
			/* Slave addess hit for Rx */
			case mskI2C_SLV_RX_MATCH_ADDR:
				I2C_State = GET_REG_STATE;
				SN_I2C0->CTRL  |= mskI2C_ACK;						//ACK
				break;
			/* Slave addess hit for Tx */
			case mskI2C_SLV_TX_MATCH_ADDR:
				I2C_State = SEND_DATA_STATE;
				SN_I2C0->TXDATA  = I2C_SendData[I2C_SendDataLen++];
				SN_GPIO0->DATA_b.DATA8 = 0x01;
				IIC_HeatTimer = 0;
				break;

			/* Received ACK */	
			case mskI2C_ACK_STAT_RECEIVED_ACK:
				SN_I2C0->TXDATA = I2C_SendData[I2C_SendDataLen++];
				break;
			/* Received NACK */	
			case mskI2C_NACK_STAT_RECEIVED_NACK:
				SN_I2C0->CTRL  |= mskI2C_ACK;							//For release SCL and SDA
				break;
			case mskI2C_RX_DN_HANDSHAKE:
				//        if(I2C_State == GET_REG_STATE)
				//        {
				//          I2C_Reg = SN_I2C0->RXDATA;
				//          I2C_State = GET_DATA_STATE;
				//        }
				//        else
				{
				if(I2C_ReceiveDataLen < 128)
				{
					I2C_ReceiveData[I2C_ReceiveDataLen++] = SN_I2C0->RXDATA ;
				}
				else 
				{
					I2C_ReceiveDataLen = 0;
				}
				}
				SN_I2C0->CTRL  |= mskI2C_ACK;						//ACK
				break;
			/*Error State Check*/
			default:
				wI2C_RegisterCheckError |= I2C_ERROR;
				wI2C_TotalError++;
				break;
		}
	}
}

/*****************************************************************************
* Function		: I2C0_IRQHandler 
* Description	: 
* Input			: 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void I2C0_IRQHandler(void)
{
	I2C0_Slave_IRQ();
}

