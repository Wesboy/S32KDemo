#include "LIN.h"

#define LIN_PORT 2       //调试串口端口号
#define LIN_TXD GPIO_PTE12 //输出脚
#define LIN_RXD GPIO_PTD17 //输入脚
#define LIN_BAUD 115200       //波特率

uint32_t *l_ifc_timerGetTimeIntervalCallbackHandler(uint32_t *nanoSeconds)
{
    return nanoSeconds;
}

/*****************************************************************************
**函数名称:	 	LinPin_Init
**函数功能:	 串口引脚初始化配置
**入口参数:
**返回参数:
******************************************************************************/
static inline void LinPin_Init(LPGPIO_PinType Pin)
{
    PORT_Type *Port_PRRS[] = PORT_BASE_PTRS;
    GPIO_Type *Gpio_PTRS[] = GPIO_BASE_PTRS;
    pin_settings_config_t config;
    memset(&config, 0, sizeof(pin_settings_config_t));
    config.base = Port_PRRS[Pin / 18];
    config.gpioBase = Gpio_PTRS[Pin / 18];
    config.pinPortIdx = Pin % 18;
    config.pullConfig = PORT_INTERNAL_PULL_DOWN_ENABLED;

    switch ((unsigned char)Pin)
    {
        case GPIO_PTA3:
        case GPIO_PTA2:
            config.mux = PORT_MUX_ALT6;
            break;
        case GPIO_PTB0:
        case GPIO_PTB1:
        case GPIO_PTD7:
        case GPIO_PTD6:
        case GPIO_PTC8:
        case GPIO_PTC9:
        case GPIO_PTC6:
        case GPIO_PTC7:
            config.mux = PORT_MUX_ALT2;
            break;
        case GPIO_PTE12:
        case GPIO_PTD17:
            config.mux = PORT_MUX_ALT3;
            break;
        default:
            config.mux = PORT_MUX_AS_GPIO;
            return;
    }

    PINS_Init(&config);
}

void LIN_Init(void)
{
    lin_user_config_t linUserConfig;
    lin_state_t linState;
    linUserConfig.baudRate = 19200;
    linUserConfig.nodeFunction = MASTER;
    linUserConfig.autobaudEnable = true;
    linUserConfig.timerGetTimeIntervalCallback = (lin_timer_get_time_interval_t)l_ifc_timerGetTimeIntervalCallbackHandler;

    {
        LinPin_Init(LIN_TXD);
        LinPin_Init(LIN_RXD);
    }

    LIN_DRV_Init(2, (lin_user_config_t *)&linUserConfig, (lin_state_t *)&linState);

}
