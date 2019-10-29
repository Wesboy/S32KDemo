




#ifndef __SYSTICK__H__
#define __SYSTICK__H__

void SysTickInit(void);
void SysTickDeInit(void);

void SetUserTimer(uint32_t *Timer,uint32_t T);
void ResetUserTimer(uint32_t *Timer);
u32 ReadUserTimer(const u32 * const Timer);
void Delayms(uint32_t ms);
uint32_t TimerGetSystemTimer(void);

#endif





