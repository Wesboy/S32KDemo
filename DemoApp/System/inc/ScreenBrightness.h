#ifndef _SCREEN_BRIGHTNESS_H_
#define _SCREEN_BRIGHTNESS_H_


#define ScreenBrightnessPWMChannel		5


void ScreenBrightnessInit(void);
void ScreenBrightnessProc(void);
void ScreenBrightnessIpcEventRegister(void);
void ScreenBrightnessProtocol(void);

#endif

