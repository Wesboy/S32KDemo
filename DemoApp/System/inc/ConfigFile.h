#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef enum CONFIG_OPTION
{
    LPC_CONFIG_OPTION,
    PANELLIGHT_CONFIG_OPTION,
    FAN_CONFIG_OPTION,
}CONFIG_OPTION_ENUM;

void ConfigFileInit(void);
void ConfigureFileIpcEventRegister(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CONFIGFILE_H__ */

