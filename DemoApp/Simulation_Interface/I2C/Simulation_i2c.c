#include "Simulation_i2c.h"

/////////////////////////////////////////////////////////////////////////
#include "interface.h"

#define IO_SDA GPIO_PTA2
#define IO_SCL GPIO_PTA3

#define I2C_SDA_High() IO_Write(IO_SDA, TRUE)
#define I2C_SDA_LOW() IO_Write(IO_SDA, FALSE)
#define I2C_SCL_High() IO_Write(IO_SCL, TRUE)
#define I2C_SCL_LOW() IO_Write(IO_SCL, FALSE)
#define I2C_SDA_Read() IO_Read(IO_SDA)

//////////////////////////////////////////////////////////////////////////

void Simulation_I2C_Delay()
{
    Delayms(1);
}


void Simulation_I2C_Start(void)
{
    I2C_SCL_High();
    I2C_SDA_High();
    Simulation_I2C_Delay();
    I2C_SDA_LOW();
    Simulation_I2C_Delay();
    I2C_SCL_LOW();
    Simulation_I2C_Delay();
}

void Simulation_I2C_Stop(void)
{
    I2C_SCL_High();
    I2C_SDA_LOW();
    Simulation_I2C_Delay();
    I2C_SDA_High();
    Simulation_I2C_Delay();
}

uint8_t Simulation_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        if (Byte & 0x80)
            I2C_SDA_High();
        else
        {
            I2C_SDA_LOW();
        }

        Simulation_I2C_Delay();
        I2C_SCL_High();
        Simulation_I2C_Delay();
        I2C_SCL_LOW();
        Simulation_I2C_Delay();

        if (i == 7)
        {
            I2C_SDA_High();
        }
        Byte <<= 1;

        Simulation_I2C_Delay();
    }
		return 0;
}

uint8_t Simulation_I2C_ReadByte(void)
{
    uint8_t i;
    uint8_t iValue = 0;

    for (i = 0; i < 8; i++)
    {
        iValue <<= 1;
        I2C_SCL_High();
        Simulation_I2C_Delay();

        if (I2C_SDA_Read())
        {
            iValue++;
        }

        I2C_SCL_LOW();
        Simulation_I2C_Delay();
    }
    return iValue;
}

void Simulation_I2C_ACK(void)
{
    I2C_SDA_LOW();
    Simulation_I2C_Delay();
    I2C_SCL_High();
    Simulation_I2C_Delay();
    I2C_SCL_LOW();
    Simulation_I2C_Delay();
    I2C_SDA_High();
}

void Simulation_I2C_NoACK(void)
{
    I2C_SDA_High();
    Simulation_I2C_Delay();
    I2C_SCL_High();
    Simulation_I2C_Delay();
    I2C_SCL_LOW();
    Simulation_I2C_Delay();
}

uint8_t Simulation_I2C_WaitToAck(void)
{
    uint8_t redata;

    I2C_SDA_High();
    Simulation_I2C_Delay();
    I2C_SCL_High();
    Simulation_I2C_Delay();

    if (I2C_SDA_Read())
    {
        redata = 1;
    }
    else
    {
        redata = 0;
    }

    I2C_SCL_LOW();
    Simulation_I2C_Delay();

    return redata;
}

void Simulation_I2C_WriteBytes(uint8_t Slaver_Addr, uint8_t Reg_Addr, uint8_t data, uint8_t len)
{
    Simulation_I2C_Start();
    Simulation_I2C_SendByte(Slaver_Addr & 0xFE);
    Simulation_I2C_WaitToAck();

    Simulation_I2C_SendByte(Reg_Addr);
    Simulation_I2C_WaitToAck();

    Simulation_I2C_SendByte(data);
    Simulation_I2C_WaitToAck();

    Simulation_I2C_Stop();
}

void Simulation_I2C_ReadBytes(uint8_t Slaver_Addr, uint8_t Reg_Addr, uint8_t *data, uint8_t len)
{
    Simulation_I2C_Start();
    Simulation_I2C_SendByte(Slaver_Addr & 0xFE);
    Simulation_I2C_WaitToAck();

    Simulation_I2C_SendByte(Reg_Addr);
    Simulation_I2C_WaitToAck();

    Simulation_I2C_Start();

    Simulation_I2C_SendByte(Slaver_Addr | 1);
    Simulation_I2C_WaitToAck();

    *data = Simulation_I2C_ReadByte();

    Simulation_I2C_ACK();

    Simulation_I2C_Stop();
}
