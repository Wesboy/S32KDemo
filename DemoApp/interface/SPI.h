#ifndef __SPI_H__
#define __SPI_H__

#ifndef		_SPI_GLOBAL_
	#define		SPI_GLOBAL		extern

#else
	#define		SPI_GLOBAL
	
#endif
SPI_GLOBAL void SPI0_Init(void);
SPI_GLOBAL void SPI1_Init(void);
SPI_GLOBAL void SPI2_Init(void);
SPI_GLOBAL void SPI_DeInit(uint8_t iSPI);
	
SPI_GLOBAL uint32_t LPSPI_receive(uint8_t iSPI, uint8_t iBit);
SPI_GLOBAL void LPSPI_transmit(uint8_t iSPI, uint8_t iBit, uint32_t send);
SPI_GLOBAL void LPSPI_Write(uint8_t iSPI, uint8_t iBit, const uint8_t *sendBuffer, uint16_t transferByteCount);
SPI_GLOBAL void LPSPI_transfer(uint8_t iSPI, uint8_t iBit, uint32_t* RecvBuffer,const  uint32_t sendBuffer, uint16_t transferCount);
#endif
