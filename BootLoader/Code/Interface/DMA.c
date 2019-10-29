#include "interface.h"

edma_state_t DMAController_State;

/*FUNCTION**********************************************************************
 *
 * Function Name : DMA_Init
 * Description   : DMA初始化
 *
 * Implements    : 
 *END**************************************************************************/
void DMA_Init()
{
	const edma_user_config_t DMAController1_InitConfig = {
		.chnArbitration = EDMA_ARBITRATION_FIXED_PRIORITY,
		.notHaltOnError = false,
	};
	EDMA_DRV_Init(&DMAController_State, &DMAController1_InitConfig, NULL, NULL, 0); //DMA初始化
}

/*FUNCTION**********************************************************************
 *
 * Function Name : DMA_DeInit
 * Description   : 
 *
 * Implements    : 
 *END**************************************************************************/
void DMA_DeInit(void)
{
	EDMA_DRV_Deinit();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : DMA__ChannelInit
 * Description   : DMA通道初始化
 *
 * Implements    : EDMA_DRV_ChannelInit_Activity
 *END**************************************************************************/
status_t DMA__ChannelInit(u8 instance, edma_chn_state_t *edmaChannelState,
						  const edma_channel_config_t *edmaChannelConfig, BOOL enableInt)
{
	EDMA_DRV_ChannelInit(edmaChannelState, edmaChannelConfig);
	if (!enableInt)
		INT_SYS_DisableIRQ(instance);
	EDMA_DRV_StartChannel(instance); //使能DMA通道0
	return 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : DMA_ConfigSingleTransfer
 * Description   : DMA通道传输配置
 *
 * Implements    : EDMA_DRV_ConfigSingleBlockTransfer_Activity
 *END**************************************************************************/
status_t DMA_ConfigSingleTransfer(uint8_t virtualChannel,
								  edma_transfer_type_t type,
								  uint32_t srcAddr,					 //源地址
								  uint32_t destAddr,				 //目标地址
								  edma_transfer_size_t transferSize, //传输bit
								  uint32_t dataBufferSize,			 //传输大小
								  BOOL isRestAdder)					 //是否在传输完成后重设传输地址
{
	EDMA_DRV_ConfigSingleBlockTransfer(virtualChannel, type, srcAddr, destAddr, transferSize, dataBufferSize); //地址配置
	if (isRestAdder)																						   //完成DMA传输后重设源目标地址
	{
		EDMA_DRV_SetSrcLastAddrAdjustment(virtualChannel, -dataBufferSize);  //传输成功地址重设
		EDMA_DRV_SetDestLastAddrAdjustment(virtualChannel, -dataBufferSize); //传输成功地址重设
	}
	return 0;
}
