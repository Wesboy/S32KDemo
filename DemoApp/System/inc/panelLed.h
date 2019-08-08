#ifndef _PANEL_LED_H_
#define _PANEL_LED_H_

#define  LIGHT_BASE_R   100 
#define  LIGHT_UP_R		120

#define		PanelledPWMChannel		3

void panelLedInit(void);
void panelLedproc(void);
void panelLedIpcEventRegister(void);
	

#endif

