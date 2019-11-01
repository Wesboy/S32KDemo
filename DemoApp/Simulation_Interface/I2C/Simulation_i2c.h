#ifndef __SIMULATION_I2C__H__
#define __SIMULATION_I2C__H__

typedef unsigned char uint8_t;

void Simulation_I2C_WriteBytes(uint8_t Slaver_Addr, uint8_t Reg_Addr, uint8_t data, uint8_t len);
void Simulation_I2C_ReadBytes(uint8_t Slaver_Addr, uint8_t Reg_Addr, uint8_t *data, uint8_t len);

#endif
