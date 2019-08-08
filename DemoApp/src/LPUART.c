/*
 * LPUART.c              Copyright NXP 2016
 * Description: LPUART functions
 * 2015 Sept 28 Kushal Shaw - original version AN5213;
 * 2016 Mar 17  O Romero - ported to S32K144;
 *
 */

//#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "LPUART.h"
#include "device_registers.h"           // Device header

void LPUART1_init(void)  /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */
{
  PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;    /* Ensure clk disabled for config */
  PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0x001)    /* Clock Src= 1 (SOSCDIV2_CLK) */
                               |  PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */

  LPUART1->BAUD = 0x00000004;  /* Initialize for 9600 baud, 1 stop: */
                               /* SBR=52 (0x34): baud divisor = 8M/9600/16 = ~52 */
                               /* OSR=15: Over sampling ratio = 15+1=16 */
                               /* SBNS=0: One stop bit */
                               /* BOTHEDGE=0: receiver samples only on rising edge */
                               /* M10=0: Rx and Tx use 7 to 9 bit data characters */
                               /* RESYNCDIS=0: Resync during rec'd data word supported */
                               /* LBKDIE, RXEDGIE=0: interrupts disable */
                               /* TDMAE, RDMAE, TDMAE=0: DMA requests disabled */
                               /* MAEN1, MAEN2,  MATCFG=0: Match disabled */

  LPUART1->CTRL=0x000C0000;    /* Enable transmitter & receiver, no parity, 8 bit char: */
                               /* RE=1: Receiver enabled */
                               /* TE=1: Transmitter enabled */
                               /* PE,PT=0: No hw parity generation or checking */
                               /* M7,M,R8T9,R9T8=0: 8-bit data characters*/
                               /* DOZEEN=0: LPUART enabled in Doze mode */
                               /* ORIE,NEIE,FEIE,PEIE,TIE,TCIE,RIE,ILIE,MA1IE,MA2IE=0: no IRQ*/
                               /* TxDIR=0: TxD pin is input if in single-wire mode */
                               /* TXINV=0: TRansmit data not inverted */
                               /* RWU,WAKE=0: normal operation; rcvr not in statndby */
                               /* IDLCFG=0: one idle character */
                               /* ILT=0: Idle char bit count starts after start bit */
                               /* SBK=0: Normal transmitter operation - no break char */
                               /* LOOPS,RSRC=0: no loop back */

//LPUART1->CTRL|=0x1<<28;

}

void LPUART1_transmit_char(char send) {    /* Function to Transmit single Char */
  while((LPUART1->STAT & LPUART_STAT_TDRE_MASK)>>LPUART_STAT_TDRE_SHIFT==0);
                                   /* Wait for transmit buffer to be empty */
  LPUART1->DATA=send;              /* Send data */
}

void LPUART1_transmit_string(char data_string[])  {  /* Function to Transmit whole string */
  uint32_t i=0;
  while(data_string[i] != '\0')  {           /* Send chars one at a time */
    LPUART1_transmit_char(data_string[i]);
    i++;
  }
}

char LPUART1_receive_char(void) {    /* Function to Receive single Char */
  static char receive;
  while((LPUART1->STAT & LPUART_STAT_RDRF_MASK)>>LPUART_STAT_RDRF_SHIFT==0);
                                     /* Wait for received buffer to be full */
  receive = LPUART1->DATA;            /* Read received data*/
  return receive;
}

void LPUART1_receive_and_echo_char(void)  {  /* Function to echo received char back */
  char send = LPUART1_receive_char();        /* Receive Char */
  LPUART1_transmit_char(send);               /* Transmit same char back to the sender */
  LPUART1_transmit_char('\n');               /* New line */
  LPUART1_transmit_char('\r');               /* Return */
}
