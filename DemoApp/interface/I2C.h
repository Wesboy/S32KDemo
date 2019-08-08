#ifndef __I2C__H__
#define __I2C__H__

bool Interface_I2C_SlaveAddr_Read(u8 adder,u8 SlaveAddr,u8 *buffer,u8 len);
bool Interface_I2C_Read(u8 adder,u8 *buffer,u8 len);
void Interface_I2C_Write(u8 adder,u8 *buffer,u8 len);
u32 I2C_TransferBuff(u8 slaveAddress,u8 *buffer,u8 len);
u32 I2C_GetMasterState(void);
void I2CMasterDeInit(void);
void I2CMasterInit(u32 baud);
#endif
