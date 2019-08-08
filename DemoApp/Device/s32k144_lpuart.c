

#include "s32k144_lpuart.h"

#include "device_registers.h"


#include "clock_manager.h"
/*!
 * @brief Sends the LPUART 8-bit character.
 *
 * This functions sends an 8-bit character.
 *
 *
 * @param base LPUART Instance
 * @param data     data to send (8-bit)
 */
	void lpuart_Putchar(LPUART_Type * base, uint8_t data)
{
  while((base->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);
                                   /* Wait for transmit buffer to be empty */
  base->DATA=data;              /* Send data */

}
/*******************************************************************************
 * Code
 ******************************************************************************/
static  void lpuaetSetBaudRate(LPUART_Type *base, unsigned int desiredBaudRate)
{
	    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    uint16_t sbr, sbrTemp, i;
    uint32_t osr, tempDiff, calculatedBaud, baudDiff;
    uint32_t lpuartSourceClock;
    clock_names_t instanceClkName ;
		uint32_t baudRegValTemp;
	
		if(base==LPUART0)
		{
			instanceClkName=LPUART0_CLK;
		}
		else if(base==LPUART1)
		{
			instanceClkName=LPUART1_CLK;
		}
		else if(base==LPUART2)
		{
			instanceClkName=LPUART2_CLK;
		}
	
		
	
    /* Get the LPUART clock as configured in the clock manager */
    (void)CLOCK_SYS_GetFreq(instanceClkName, &lpuartSourceClock);

    /* Check if current instance is clock gated off. */
    DEV_ASSERT(lpuartSourceClock > 0U);
    /* Check if the desired baud rate can be configured with the current protocol clock. */
    DEV_ASSERT(lpuartSourceClock >= (desiredBaudRate * 4U));

    /* This lpuart instantiation uses a slightly different baud rate calculation
     * The idea is to use the best OSR (over-sampling rate) possible
     * Note, osr is typically hard-set to 16 in other lpuart instantiations
     * First calculate the baud rate using the minimum OSR possible (4) */
    osr = 4;
    sbr = (uint16_t)(lpuartSourceClock / (desiredBaudRate * osr));
    calculatedBaud = (lpuartSourceClock / (osr * sbr));

    if (calculatedBaud > desiredBaudRate)
    {
        baudDiff = calculatedBaud - desiredBaudRate;
    }
    else
    {
        baudDiff = desiredBaudRate - calculatedBaud;
    }

    /* loop to find the best osr value possible, one that generates minimum baudDiff
     * iterate through the rest of the supported values of osr */
    for (i = 5U; i <= 32U; i++)
    {
        /* calculate the temporary sbr value   */
        sbrTemp = (uint16_t)(lpuartSourceClock / (desiredBaudRate * i));
        /* calculate the baud rate based on the temporary osr and sbr values */
        calculatedBaud = (lpuartSourceClock / (i * sbrTemp));

        if (calculatedBaud > desiredBaudRate)
        {
            tempDiff = calculatedBaud - desiredBaudRate;
        }
        else
        {
            tempDiff = desiredBaudRate - calculatedBaud;
        }

        if (tempDiff <= baudDiff)
        {
            baudDiff = tempDiff;
            osr = i;  /* update and store the best osr value calculated */
            sbr = sbrTemp;  /* update store the best sbr value calculated */
        }
    }

//    /* Check if osr is between 4x and 7x oversampling.
//     * If so, then "BOTHEDGE" sampling must be turned on */
//    if ((osr > 3U) && (osr < 8U))
//    {
//        LPUART_SetBothEdgeSamplingCmd(base, true);
//    }

//    /* program the osr value (bit value is one less than actual value) */
//    LPUART_SetOversamplingRatio(base, (osr - 1U));

//    /* write the sbr value to the BAUD registers */
//    LPUART_SetBaudRateDivisor(base, sbr);


    baudRegValTemp = base->BAUD;
		baudRegValTemp &= ~(LPUART_BAUD_OSR_MASK);
		baudRegValTemp &= ~(LPUART_BAUD_SBR_MASK);
    baudRegValTemp |= LPUART_BAUD_OSR(osr - 1U);
    baudRegValTemp |= sbr & LPUART_BAUD_SBR_MASK;
   
    base->BAUD = baudRegValTemp;
  
}
/*!
 * @brief Enable/Disable the LPUART transmitter.
 *
 * This function enables or disables the LPUART transmitter, based on the
 * parameter received.
 *
 *
 * @param base LPUART base pointer.
 * @param enable Enable(true) or disable(false) transmitter.
 */
static inline void LPUART_SetTransmitterCmd(LPUART_Type * base, bool enable)
{
    base->CTRL = (base->CTRL & ~LPUART_CTRL_TE_MASK) | ((enable ? 1UL : 0UL) << LPUART_CTRL_TE_SHIFT);
    /* Wait for the register write operation to complete */
    while((bool)((base->CTRL & LPUART_CTRL_TE_MASK) != 0U) != enable) {}
}

/*!
 * @brief Enable/Disable the LPUART receiver.
 *
 * This function enables or disables the LPUART receiver, based on the
 * parameter received.
 *
 *
 * @param base LPUART base pointer
 * @param enable Enable(true) or disable(false) receiver.
 */
static inline void LPUART_SetReceiverCmd(LPUART_Type * base, bool enable)
{
    base->CTRL = (base->CTRL & ~LPUART_CTRL_RE_MASK) | ((enable ? 1UL : 0UL) << LPUART_CTRL_RE_SHIFT);
    /* Wait for the register write operation to complete */
    while((bool)((base->CTRL & LPUART_CTRL_RE_MASK) != 0U) != enable) {}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_Init
 * Description   : This function initializes a LPUART instance for operation.
 * This function will initialize the run-time state structure to keep track of
 * the on-going transfers, ungate the clock to the LPUART module, initialize the
 * module to user defined settings and default settings, configure the IRQ state
 * structure and enable the module-level interrupt to the core, and enable the
 * LPUART module transmitter and receiver.
 * The following is an example of how to set up the lpuart_state_t and the
 * lpuart_user_config_t parameters and how to call the LPUART_DRV_Init function
 * by passing in these parameters:
 *    lpuart_user_config_t lpuartConfig;
 *    lpuartConfig.baudRate = 9600;
 *    lpuartConfig.bitCountPerChar = LPUART_8_BITS_PER_CHAR;
 *    lpuartConfig.parityMode = LPUART_PARITY_DISABLED;
 *    lpuartConfig.stopBitCount = LPUART_ONE_STOP_BIT;
 *    lpuartConfig.transferType = LPUART_USING_INTERRUPTS;
 *    lpuart_state_t lpuartState;
 *    LPUART_DRV_Init(instance, &lpuartState, &lpuartConfig);
 *
 * Implements    : LPUART_DRV_Init_Activity
 *END**************************************************************************/
void lpuart_device_Init(LPUART_Type* base,const tlpuart_config  lpuartConfig)
{
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);
    DEV_ASSERT(lpuartStatePtr != NULL);
    DEV_ASSERT(lpuartUserConfig != NULL);
    /* initialize the parameters of the LPUART config structure with desired data */
    lpuaetSetBaudRate(base, lpuartConfig.baudRate);
    if (lpuartConfig.bitCountPerChar == LPUART_10_BITS_PER_CHAR)
    {
        base->BAUD = (base->BAUD & ~LPUART_BAUD_M10_MASK) | ((uint32_t)1U << LPUART_BAUD_M10_SHIFT);
    }
    else
    {
        /* config 8-bit (M=0) or 9-bits (M=1) */
        base->CTRL = (base->CTRL & ~LPUART_CTRL_M_MASK) | ((uint32_t)lpuartConfig.bitCountPerChar << LPUART_CTRL_M_SHIFT);
        /* clear M10 to make sure not 10-bit mode */
        base->BAUD &= ~LPUART_BAUD_M10_MASK;
    }
    base->CTRL = (base->CTRL & ~LPUART_CTRL_PE_MASK) | (((uint32_t)lpuartConfig.parityMode >> 1U) << LPUART_CTRL_PE_SHIFT);
    base->CTRL = (base->CTRL & ~LPUART_CTRL_PT_MASK) | (((uint32_t)lpuartConfig.parityMode & 1U) << LPUART_CTRL_PT_SHIFT);
    base->BAUD = (base->BAUD & ~LPUART_BAUD_SBNS_MASK) | ((uint32_t)lpuartConfig.stopBitCount << LPUART_BAUD_SBNS_SHIFT);

    /* finally, enable the LPUART transmitter and receiver */

			LPUART_SetTransmitterCmd(base,true);
			   LPUART_SetReceiverCmd(base,true);

base->CTRL = (base->CTRL & ~LPUART_CTRL_RIE_MASK) | ((  1UL ) << LPUART_CTRL_RIE_SHIFT);
			

}

 void lpuart_device_DeInit(LPUART_Type* base)
 {
	 LPUART_SetTransmitterCmd(base,false);
	LPUART_SetReceiverCmd(base,false);
 }
 