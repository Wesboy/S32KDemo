#include "Interface.h"

uint16_t CurrentCaptureVal = 0;
uint16_t PriorCaptureVal = 0;
uint16_t DeltaCapture = 0;

void FTM_IC_init(void) {
	
  FTM1->CONTROLS[0].CnSC = 0x0000000C;  /* FTM1 ch0: Input Capture rising or falling edge */
                                        /* CHIE (Chan Interrupt Ena) = 0 (default) */
                                        /* MSB:MSA (chan Mode Select)=0b00, Input Capture */
                                        /* ELSB:ELSA (ch Edge/Level Select)=0b11, rise or fall*/
	FTM1->CONTROLS[1].CnSC = 0x0000000C; 
	FTM1->CONTROLS[2].CnSC = 0x0000000C; 
	FTM1->CONTROLS[3].CnSC = 0x0000000C; 
//	FTM0->CONTROLS[6].CnSC = 0x0000000C; 
}

void FTM_input_capture(void) {
  if (1==((FTM1->CONTROLS[0].CnSC & FTM_CnSC_CHF_MASK)>>FTM_CnSC_CHF_SHIFT)) {
	                                               /* If chan flag is set */
  FTM1->CONTROLS[0].CnSC &= ~FTM_CnSC_CHF_MASK;    /* Clear flag: read reg then set CHF=0 */
  PriorCaptureVal = CurrentCaptureVal;             /* Record value of prior capture */
  CurrentCaptureVal = FTM1->CONTROLS[0].CnV;       /* Record value of current capture */
  DeltaCapture = CurrentCaptureVal - PriorCaptureVal;
                                  /* Will be 6250 clocks (100 msec) if connected to FTM0 CH0 */
  }
}
