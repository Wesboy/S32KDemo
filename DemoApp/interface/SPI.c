#define _SPI_GLOBAL_

#include "Interface.h"
#include "status.h"

#include "lpspi_hw_access.h"

#define Master 1

static lpspi_state_t lpspistate;

uint8_t rxBuff[30];



#define SPIRxFifoCount 1000
#define SPITxFifoCount 1000

NEW_FIFO(SPI0RxFifo, SPIRxFifoCount, uint8_t, uint16_t);
NEW_FIFO(SPI0TxFifo, SPIRxFifoCount, uint8_t, uint16_t);
NEW_FIFO(SPI1RxFifo, SPIRxFifoCount, uint8_t, uint16_t);
NEW_FIFO(SPI1TxFifo, SPIRxFifoCount, uint8_t, uint16_t);
NEW_FIFO(SPI2RxFifo, SPIRxFifoCount, uint8_t, uint16_t);
NEW_FIFO(SPI2TxFifo, SPIRxFifoCount, uint8_t, uint16_t);

static pin_settings_config_t g_pin_mux_InitConfigArr_SPI2[4] = {
	{
		.base = PORTE,
		.pinPortIdx = 10u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTE,
		.pinPortIdx = 15u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTE,
		.pinPortIdx = 16u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTC,
		.pinPortIdx = 1u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},
};

static pin_settings_config_t g_pin_mux_InitConfigArr_SPI1[4] = {
	{
		.base = PORTD,
		.pinPortIdx = 3u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTD,
		.pinPortIdx = 2u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTD,
		.pinPortIdx = 0u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTD,
		.pinPortIdx = 1u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},
};

static pin_settings_config_t g_pin_mux_InitConfigArr_SPI0[4] = {
	{
		.base = PORTB,
		.pinPortIdx = 4u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTB,
		.pinPortIdx = 5u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTD,
		.pinPortIdx = 15u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},

	{
		.base = PORTD,
		.pinPortIdx = 16u,
		.pullConfig = PORT_INTERNAL_PULL_NOT_ENABLED,
		.passiveFilter = false,
		.driveSelect = PORT_LOW_DRIVE_STRENGTH,
		.mux = PORT_MUX_ALT3,
		.pinLock = false,
		.intConfig = PORT_DMA_INT_DISABLED,
		.clearIntFlag = false,
		.gpioBase = NULL,
	},
};

void SPI0RxFifoMsg(uint8_t data)
{
	FIFO_PUT_ONE_INT(SPI0RxFifo, data);
}

bool SPI0TxFifoMsg(uint8_t *data)
{
	if (FIFO_GET_COUNT(SPI0TxFifo))
	{
		FIFO_GET_ONE_INT(SPI0TxFifo, data);
		return true;
	}
	return false;
}

void SPI1RxFifoMsg(uint8_t data)
{
	FIFO_PUT_ONE_INT(SPI1RxFifo, data);
}

bool SPI1TxFifoMsg(uint8_t *data)
{
	if (FIFO_GET_COUNT(SPI1TxFifo))
	{
		FIFO_GET_ONE_INT(SPI1TxFifo, data);
		return true;
	}
	return false;
}

void SPI2RxFifoMsg(uint8_t data)
{
	FIFO_PUT_ONE_INT(SPI2RxFifo, data);
}

bool SPI2TxFifoMsg(uint8_t *data)
{
	if (FIFO_GET_COUNT(SPI2TxFifo))
	{
		FIFO_GET_ONE_INT(SPI2TxFifo, data);
		return true;
	}
	return false;
}

static void SPI0FifoInit(void)
{
	FIFO_INIT(SPI0RxFifo);
	FIFO_INIT(SPI0TxFifo);
}

static void SPI1FifoInit(void)
{
	FIFO_INIT(SPI1RxFifo);
	FIFO_INIT(SPI1TxFifo);
}

static void SPI2FifoInit(void)
{
	FIFO_INIT(SPI2RxFifo);
	FIFO_INIT(SPI2TxFifo);
}

void lpspi_callback(void *lpspiState, spi_event_t event, void *userData)
{
	//	 printf("\r\nhhhlpspi_callback");
}

void SPI0_Init(void)
{
	const lpspi_master_config_t Send_MasterConfig0 =
		{
			.bitsPerSec = 2000000U,
			.whichPcs = LPSPI_PCS1,
			.pcsPolarity = LPSPI_ACTIVE_LOW,

			.isPcsContinuous = true,
			.bitcount = 8U,
			.lpspiSrcClk = 8000000U,
			.clkPhase = LPSPI_CLOCK_PHASE_2ND_EDGE,
			.clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
			.lsbFirst = false,
			.transferType = LPSPI_USING_INTERRUPTS,
			.rxDMAChannel = 255,
			.txDMAChannel = 255,
			.callback = lpspi_callback,
			.callbackParam = NULL,
		};

//	const lpspi_slave_config_t Receive_SlaveConfig0 =
//		{
//			.pcsPolarity = LPSPI_ACTIVE_HIGH,
//			.bitcount = 8U,
//			.clkPhase = LPSPI_CLOCK_PHASE_1ST_EDGE,
//			.whichPcs = LPSPI_PCS1,
//			.clkPolarity = LPSPI_SCK_ACTIVE_LOW,
//			.lsbFirst = false,
//			.transferType = LPSPI_USING_INTERRUPTS,
//			.rxDMAChannel = 255,
//			.txDMAChannel = 255,
//			.callback = NULL,
//			.callbackParam = NULL,
//		};
	PORTB->PCR[4] &= 0x0; //Clear set spi clk
	PORTB->PCR[5] &= 0x0;
	PORTD->PCR[15] &= 0x0;
	PORTD->PCR[16] &= 0x0;
	PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTB->PCR[4] |= PORT_PCR_MUX(3);
	PORTB->PCR[5] |= PORT_PCR_MUX(3);
	PORTD->PCR[15] |= PORT_PCR_MUX(3);
	PORTD->PCR[16] |= PORT_PCR_MUX(3);

	PCC->PCCn[PCC_LPSPI0_INDEX] = 0;		  /* Disable clocks to modify PCS ( default) */
	PCC->PCCn[PCC_LPSPI0_INDEX] = 0xC6000000; /* Enable PCS=SPLL_DIV2 */

	PINS_DRV_Init(4, g_pin_mux_InitConfigArr_SPI0);

	lpspistate.callback = lpspi_callback;

	LPSPI_DRV_MasterInit(0, &lpspistate, &Send_MasterConfig0);
	LPSPI_DRV_MasterSetDelay(0, 1, 1, 1);

	//	  LPSPI_DRV_SlaveInit(1,&lpspistate,&Receive_SlaveConfig0);
	SPI0FifoInit();
}

void SPI1_Init(void)
{
	const lpspi_master_config_t Send_MasterConfig0 =
		{
			.bitsPerSec = 2000000U,
			.whichPcs = LPSPI_PCS1,
			.pcsPolarity = LPSPI_ACTIVE_LOW,
			.isPcsContinuous = true,
			.bitcount = 8U,
			.lpspiSrcClk = 112000000U,
			//.lpspiSrcClk = 8000000U,
			.clkPhase = LPSPI_CLOCK_PHASE_2ND_EDGE,
			.clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
			.lsbFirst = false,
			.transferType = LPSPI_USING_INTERRUPTS,
			.rxDMAChannel = 255,
			.txDMAChannel = 255,
			.callback = lpspi_callback,
			.callbackParam = NULL,
		};

//	const lpspi_slave_config_t Receive_SlaveConfig0 =
//		{
//			.pcsPolarity = LPSPI_ACTIVE_HIGH,
//			.bitcount = 8U,
//			.clkPhase = LPSPI_CLOCK_PHASE_1ST_EDGE,
//			.whichPcs = LPSPI_PCS1,
//			.clkPolarity = LPSPI_SCK_ACTIVE_LOW,
//			.lsbFirst = false,
//			.transferType = LPSPI_USING_INTERRUPTS,
//			.rxDMAChannel = 255,
//			.txDMAChannel = 255,
//			.callback = NULL,
//			.callbackParam = NULL,
//		};
	PORTD->PCR[0] &= 0x0; //Clear set spi clk
	PORTD->PCR[1] &= 0x0;
	PORTD->PCR[2] &= 0x0;
	PORTD->PCR[3] &= 0x0;
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PORTD->PCR[0] |= PORT_PCR_MUX(3);
	PORTD->PCR[1] |= PORT_PCR_MUX(3);
	PORTD->PCR[2] |= PORT_PCR_MUX(3);
	PORTD->PCR[3] |= PORT_PCR_MUX(3);

	PCC->PCCn[PCC_LPSPI1_INDEX] = 0;		  /* Disable clocks to modify PCS ( default) */
	PCC->PCCn[PCC_LPSPI1_INDEX] = 0xC6000000; /* Enable PCS=SPLL_DIV2 */

	PINS_DRV_Init(4, g_pin_mux_InitConfigArr_SPI1);

	lpspistate.callback = lpspi_callback;

	LPSPI_DRV_MasterInit(1, &lpspistate, &Send_MasterConfig0);
	LPSPI_DRV_MasterSetDelay(1, 1, 1, 1);

	SPI1FifoInit();
}

void SPI2_Init(void)
{
	const lpspi_master_config_t Send_MasterConfig0 =
		{
			.bitsPerSec = 2000000U,
			.whichPcs = LPSPI_PCS1,
			.pcsPolarity = LPSPI_ACTIVE_LOW,

			.isPcsContinuous = true,
			.bitcount = 8U,
			.lpspiSrcClk = 112000000U,
			//.lpspiSrcClk = 8000000U,
			.clkPhase = LPSPI_CLOCK_PHASE_2ND_EDGE,
			.clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
			.lsbFirst = false,
			.transferType = LPSPI_USING_INTERRUPTS,
			.rxDMAChannel = 255,
			.txDMAChannel = 255,
			.callback = lpspi_callback,
			.callbackParam = NULL,
		};

//	const lpspi_slave_config_t Receive_SlaveConfig0 =
//		{
//			.pcsPolarity = LPSPI_ACTIVE_LOW,
//			.bitcount = 8U,
//			.clkPhase = LPSPI_CLOCK_PHASE_2ND_EDGE,
//			.whichPcs = LPSPI_PCS1,
//			.clkPolarity = LPSPI_SCK_ACTIVE_HIGH,
//			.lsbFirst = false,
//			.transferType = LPSPI_USING_INTERRUPTS,
//			.rxDMAChannel = 255,
//			.txDMAChannel = 255,
//			.callback = NULL,
//			.callbackParam = NULL,
//		};
	PORTE->PCR[10] &= 0x0; //Clear set spi clk
	PORTE->PCR[15] &= 0x0;
	PORTE->PCR[16] &= 0x0;
	PORTC->PCR[1] &= 0x0;

	PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
	PORTE->PCR[10] |= PORT_PCR_MUX(3);
	PORTE->PCR[15] |= PORT_PCR_MUX(3);
	PORTE->PCR[16] |= PORT_PCR_MUX(3);
	PORTC->PCR[1] |= PORT_PCR_MUX(3);

	PCC->PCCn[PCC_LPSPI2_INDEX] = 0;		  /* Disable clocks to modify PCS ( default) */
	PCC->PCCn[PCC_LPSPI2_INDEX] = 0xC6000000; /* Enable PCS=SPLL_DIV2 */

	PINS_DRV_Init(4, g_pin_mux_InitConfigArr_SPI2);

	lpspistate.callback = lpspi_callback;

	LPSPI_DRV_MasterInit(2, &lpspistate, &Send_MasterConfig0);
	LPSPI_DRV_MasterSetDelay(2, 1, 1, 1);

	SPI2FifoInit();
}

void SPI_DeInit(uint8_t iSPI)
{
	if(iSPI < 3)
		LPSPI_DRV_MasterDeinit(iSPI);
}

void LPSPI_transmit(uint8_t iSPI, uint8_t iBit, uint32_t send)
{
	LPSPI_Type *gSPIBUS = NULL;

	if (iSPI >= 3)
		return;

	if(0 == iSPI)
	{
		gSPIBUS = LPSPI0;
	}
	else if(1 == iSPI)
	{
		gSPIBUS = LPSPI1;
	}
	else if(2 == iSPI)
	{
		gSPIBUS = LPSPI2;
	}

	while ((gSPIBUS->SR & LPSPI_SR_TDF_MASK) >> LPSPI_SR_TDF_SHIFT == 0);
	/* Wait for Tx FIFO available */
	switch(iBit)
	{
	case 8:
		gSPIBUS->TDR = send & 0xFF; /* Transmit data */
		break;
	case 16:
		gSPIBUS->TDR = send & 0xFFFF; /* Transmit data */
		break;
	case 32:
		gSPIBUS->TDR = send; /* Transmit data */
		break;
	}
	gSPIBUS->SR |= LPSPI_SR_TDF_MASK; /* Clear TDF flag */
}


uint32_t LPSPI_receive(uint8_t iSPI, uint8_t iBit)
{
	LPSPI_Type *gSPIBUS = NULL;
	uint32_t receive = 0;

	if(iSPI >= 3)
		return 0;

	if (0 == iSPI)
	{
		gSPIBUS = LPSPI0;
	}
	else if (1 == iSPI)
	{
		gSPIBUS = LPSPI1;
	}
	else if (2 == iSPI)
	{
		gSPIBUS = LPSPI2;
	}

	while ((gSPIBUS->SR & LPSPI_SR_RDF_MASK) >> LPSPI_SR_RDF_SHIFT == 0);
	receive = gSPIBUS->RDR;			 /* Read received data */
	gSPIBUS->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */

	switch (iBit)
	{
	case 8:
		receive &= 0xFF; /* Read received data */
		break;
	case 16:
		receive &= 0xFFFF; /* Read received data */
		break;
	case 32:
		receive &= 0xFFFFFFFF; /* Read received data */
		break;
	}

	return receive;
}

void LPSPI_transfer(uint8_t iSPI, uint8_t iBit, uint32_t* RecvBuffer,const  uint32_t sendBuffer, uint16_t transferCount)
{
	uint8_t count;
	for(count=0; count<transferCount; count++)
	{
		LPSPI_transmit(iSPI, iBit, sendBuffer);
		*RecvBuffer = LPSPI_receive(iSPI, iBit);
	}
}

void LPSPI_Write(uint8_t iSPI, uint8_t iBit, const uint8_t *sendBuffer, uint16_t transferByteCount)
{
	uint8_t count;
	for (count = 0; count < transferByteCount; count++)
	{
		LPSPI_transmit(iSPI, iBit, sendBuffer[count]);
	}
}

void LPSPI2_IRQHandler(void)
{
	uint32_t receive = 0;

	/* If an error is detected the transfer will be aborted */
	if ((bool)LPSPI_GetStatusFlag(LPSPI2, LPSPI_TRANSMIT_ERROR))
	{
	}
	if (LPSPI_GetStatusFlag(LPSPI2, LPSPI_RECEIVE_ERROR))
	{
	}

	/* Receive data */
	if (LPSPI_GetStatusFlag(LPSPI2, LPSPI_RX_DATA_FLAG))
	{
		receive = LPSPI2->RDR; /* Read received data */

		SPI2RxFifoMsg(receive);
		printf("\r\nSPI2 Receive Data: %X", receive);

		LPSPI2->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
}

void LPSPI1_IRQHandler(void)
{
	uint32_t receive = 0;

	/* If an error is detected the transfer will be aborted */
	if ((bool)LPSPI_GetStatusFlag(LPSPI1, LPSPI_TRANSMIT_ERROR))
	{
	}
	if (LPSPI_GetStatusFlag(LPSPI1, LPSPI_RECEIVE_ERROR))
	{
	}

	/* Receive data */
	if (LPSPI_GetStatusFlag(LPSPI1, LPSPI_RX_DATA_FLAG))
	{
		receive = LPSPI1->RDR; /* Read received data */

		SPI1RxFifoMsg(receive);
		printf("\r\nSPI1 Receive Data: %X", receive);

		LPSPI1->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
}

void LPSPI0_IRQHandler(void)
{
	uint32_t receive = 0;

	/* If an error is detected the transfer will be aborted */
	if ((bool)LPSPI_GetStatusFlag(LPSPI0, LPSPI_TRANSMIT_ERROR))
	{
	}
	if (LPSPI_GetStatusFlag(LPSPI0, LPSPI_RECEIVE_ERROR))
	{
	}

	/* Receive data */
	if (LPSPI_GetStatusFlag(LPSPI0, LPSPI_RX_DATA_FLAG))
	{
		receive = LPSPI0->RDR; /* Read received data */

		SPI1RxFifoMsg(receive);
		printf("\r\nSPI1 Receive Data: %X", receive);

		LPSPI0->SR |= LPSPI_SR_RDF_MASK; /* Clear RDF flag */
	}
}
