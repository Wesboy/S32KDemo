#ifndef __SPI_H__
#define __SPI_H__

#include "fly_types.h"

#ifndef		_SPI_GLOBAL_
	#define		SPI_GLOBAL		extern

#else
	#define		SPI_GLOBAL
	
#endif
SPI_GLOBAL void SPI1_Init(void);
SPI_GLOBAL void SPI2_Init(void);
SPI_GLOBAL void SPI1_DeInit(void);
SPI_GLOBAL void SPI2_DeInit(void);
	
	
SPI_GLOBAL void Fly_SPI_MasterWrite(const uint8_t * sendBuffer, uint16_t transferByteCount);
SPI_GLOBAL void Fly_SPI_MasterRead(uint32_t subAddr, uint8_t *buffer, uint32_t len);
SPI_GLOBAL void Fly_SPI_MasterReadBack(uint32_t subAddr, uint8_t *buffer, uint32_t len);
	
SPI_GLOBAL uint16_t LPSPI1_receive_16bits(void);
SPI_GLOBAL void LPSPI1_transmit_16bits(uint16_t send);
SPI_GLOBAL void LPSPI1_Write_16bit(const uint16_t * sendBuffer, uint16_t transferByteCount);
	
SPI_GLOBAL uint16_t LPSPI2_receive_16bits(void);
SPI_GLOBAL void LPSPI2_transmit_16bits(uint16_t send);
SPI_GLOBAL void LPSPI2_transmit_32bits(uint32_t send);
SPI_GLOBAL uint32_t LPSPI2_receive_32bits(void);
SPI_GLOBAL void LPSPI2_WriteRead_32bit(uint32_t * RecvBuffer,const  uint32_t sendBuffer, uint16_t transferCount);

#endif
