
#include "Interface.h"

lpi2c_master_state_t lpi2c_master_state;

///////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************
***	函数： i2cmasterCallback
**  功能： 
***	备注： 
******************************************************************/
void i2cmasterCallback(i2c_master_event_t event,void *data)
{
	

	if(lpi2c_master_state.status)
	{
		printf("\r\nI2C0 status=%x",lpi2c_master_state.status);
	}
	 
	 if(lpi2c_master_state.status==STATUS_I2C_RECEIVED_NACK)
	{
		printf("\r\nNACK signal received");
	}
	else if(lpi2c_master_state.status==STATUS_I2C_TX_UNDERRUN)
	{
		printf("\r\nTX underrun error");
	}
	else if(lpi2c_master_state.status==STATUS_I2C_RX_OVERRUN)
	{
		printf("\r\nRX overrun error");
	}
	else if(lpi2c_master_state.status==STATUS_I2C_ARBITRATION_LOST)
	{
		printf("\r\nArbitration lost");
	}
	else if(lpi2c_master_state.status==STATUS_I2C_ABORTED)
	{
		printf("\r\nA transfer was aborted");
	}

}

/*****************************************************************
***	函数： I2CMasterInit
**  功能： 
***	备注： 
******************************************************************/
void I2CMasterInit(uint32_t baud)
{
	/*串口时钟开启*/
	module_clk_config_t module_clk_config;
	module_clk_config.clkSrcFreq =LOWEST_FREQUENCY;//
	module_clk_config.clkSrcType=XOSC_CLK_SRC_TYPE;
	CLOCK_DRV_SetModuleClock(LPI2C0_CLK,&module_clk_config);

	pin_settings_config_t g_pin_mux_InitConfigArr[2] = 
	{
		{
			.base          = PORTA,
			.pinPortIdx    = 3u,
			.pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
			.passiveFilter = false,
			.driveSelect   = PORT_LOW_DRIVE_STRENGTH,
			.mux           = PORT_MUX_ALT3,
			.pinLock       = false,
			.intConfig     = PORT_DMA_INT_DISABLED,
			.clearIntFlag  = false,
			.gpioBase      = NULL,
		},
		{
			.base          = PORTA,
			.pinPortIdx    = 2u,
			.pullConfig    = PORT_INTERNAL_PULL_NOT_ENABLED,
			.passiveFilter = false,
			.driveSelect   = PORT_LOW_DRIVE_STRENGTH,
			.mux           = PORT_MUX_ALT3,
			.pinLock       = false,
			.intConfig     = PORT_DMA_INT_DISABLED,
			.clearIntFlag  = false,
			.gpioBase      = NULL,
		}
	};

	PINS_DRV_Init(2, g_pin_mux_InitConfigArr);

	lpi2c_master_user_config_t lpi2c_master_user_config;
	lpi2c_master_user_config.baudRate=baud;
	lpi2c_master_user_config.operatingMode=LPI2C_FAST_MODE;
	lpi2c_master_user_config.slaveAddress=0;
	lpi2c_master_user_config.transferType=LPI2C_USING_INTERRUPTS;
	lpi2c_master_user_config.masterCallback=i2cmasterCallback;				
	LPI2C_DRV_MasterInit(0,&lpi2c_master_user_config,&lpi2c_master_state);
	INT_SYS_EnableIRQ(LPI2C0_Master_IRQn);
	INT_SYS_SetPriority(LPI2C0_Master_IRQn, 5);	
}

/*****************************************************************
***	函数： I2CMasterDeInit
**  功能： 
***	备注： 
******************************************************************/
void I2CMasterDeInit(void)
{
		LPI2C_DRV_MasterDeinit(0);
		INT_SYS_DisableIRQ(LPI2C0_Master_IRQn);
}
/*****************************************************************
***	函数： I2C_GetMasterState
**  功能： 获取I2C当前状态
***	备注： 
******************************************************************/
uint32_t I2C_GetMasterState()
{
	return lpi2c_master_state.status;
}
/*****************************************************************
***	函数： I2C_TransferBuff
**  功能： 传输I2C数据功能的底层接口函数
***	备注： 用slaveAddress bit0区分读写
******************************************************************/
uint32_t I2C_TransferBuff(uint8_t slaveAddress,uint8_t *buffer,uint8_t len)
{
	if(lpi2c_master_state.status==STATUS_BUSY)
		return STATUS_BUSY;
	LPI2C_DRV_MasterSetSlaveAddr(0,slaveAddress>>1,false);
	
	if(slaveAddress&0x1)	
	LPI2C_DRV_MasterReceiveData(0, buffer, len, true);
			else
	LPI2C_DRV_MasterSendData(0,buffer,len,true);
	return 0;
}
/*****************************************************************
***	函数： Interface_I2C_Write
**  功能： I2C写数据
***	备注： addr为8位地址
******************************************************************/
void Interface_I2C_Write(uint8_t addr,uint8_t *buffer,uint8_t len)
{
	while(STATUS_BUSY==I2C_GetMasterState());
	I2C_TransferBuff(addr,buffer,len);	
}
/*****************************************************************
***	函数： Interface_I2C_Read
**  功能： I2C写数据
***	备注： addr为8位
******************************************************************/
bool Interface_I2C_Read(uint8_t addr,uint8_t *buffer,uint8_t len)
{
	while(STATUS_BUSY==I2C_GetMasterState());//等待空闲

	I2C_TransferBuff((addr|0x1),buffer,len);

	while(lpi2c_master_state.status==STATUS_BUSY)
				;
	return lpi2c_master_state.status?false:true;	
}
/*****************************************************************
***	函数： Interface_I2C_SlaveAddr_Read
**  功能： I2C写数据
***	备注： addr为8位地址，SlaveAddr寄存器地址
******************************************************************/
bool Interface_I2C_SlaveAddr_Read(uint8_t addr,uint8_t SlaveAddr,uint8_t *buffer,uint8_t len)
{
	uint8_t rStartBuf[2];
	rStartBuf[0] = addr;
	rStartBuf[1] = SlaveAddr;

	while(STATUS_BUSY==I2C_GetMasterState());
	I2C_TransferBuff((addr | 0x1), rStartBuf, 2);
	while(STATUS_BUSY==I2C_GetMasterState());
	I2C_TransferBuff((addr|0x1),buffer,len);
	while(STATUS_BUSY==I2C_GetMasterState());
	return I2C_GetMasterState();	
}

/*****************************************************************
***	函数： Interface_I2C_Write
**  功能： I2C写数据
***	备注： addr为8位地址
******************************************************************/
bool Interface_I2C_SlaveAddr_Write(uint8_t addr, uint8_t *buffer, uint8_t len)
{
	while (STATUS_BUSY == I2C_GetMasterState());
	I2C_TransferBuff(addr, &addr, 1);
	while (STATUS_BUSY == I2C_GetMasterState());
	I2C_TransferBuff((addr & 0xFE), buffer, len);
	while (STATUS_BUSY == I2C_GetMasterState());
	return I2C_GetMasterState();
}
