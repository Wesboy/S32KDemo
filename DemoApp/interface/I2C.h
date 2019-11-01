#ifndef __I2C__H__
#define __I2C__H__

bool Interface_I2C_SlaveAddr_Read(uint8_t adder,uint8_t SlaveAddr,uint8_t *buffer,uint8_t len);
bool Interface_I2C_Read(uint8_t adder,uint8_t *buffer,uint8_t len);
void Interface_I2C_Write(uint8_t adder,uint8_t *buffer,uint8_t len);
uint32_t I2C_TransferBuff(uint8_t slaveAddress,uint8_t *buffer,uint8_t len);
uint32_t I2C_GetMasterState(void);
void I2CMasterDeInit(void);
void I2CMasterInit(uint32_t baud);

void Simulation_I2C_WriteBytes(uint8_t Slaver_Addr, uint8_t Reg_Addr, uint8_t data, uint8_t len);
void Simulation_I2C_ReadBytes(uint8_t Slaver_Addr, uint8_t Reg_Addr, uint8_t *data, uint8_t len);
#endif
