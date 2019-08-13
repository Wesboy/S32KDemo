
#include "Interface.h"
void PWM1ConfigDeInit()
{
	FTM_DRV_Reset(FTM0);
	FTM_DRV_Reset(FTM1);	
	FTM_DRV_Reset(FTM2);
	{
			module_clk_config_t module_clk_config;
					module_clk_config.clkSrcFreq =NO_CLOCK;//
					module_clk_config.clkSrcType=XOSC_CLK_SRC_TYPE;
					CLOCK_DRV_SetModuleClock(FTM0_CLK,&module_clk_config);
					CLOCK_DRV_SetModuleClock(FTM1_CLK,&module_clk_config);
					CLOCK_DRV_SetModuleClock(FTM2_CLK,&module_clk_config);
	}
}

void PWM1ConfigInit(void)
{

		module_clk_config_t module_clk_config;
		module_clk_config.clkSrcFreq =LOWEST_FREQUENCY;//
		module_clk_config.clkSrcType=XOSC_CLK_SRC_TYPE;
		CLOCK_DRV_SetModuleClock(FTM0_CLK,&module_clk_config);
		CLOCK_DRV_SetModuleClock(FTM2_CLK,&module_clk_config);
		CLOCK_DRV_SetModuleClock(FTM3_CLK,&module_clk_config);

		pin_settings_config_t config;
		memset(&config,0,sizeof(pin_settings_config_t));
	
		config.direction=GPIO_OUTPUT_DIRECTION;
		config.pullConfig=PORT_INTERNAL_PULL_DOWN_ENABLED;	

		config.mux=PORT_MUX_ALT2;	
		config.base=PORTD;
		config.gpioBase=PTD;
		config.pinPortIdx=10;  //FTM2_CH0   PWM1
		PINS_Init(&config);

		config.mux=PORT_MUX_ALT2;
		config.base=PORTD;
		config.gpioBase=PTD;
		config.pinPortIdx=11;  //FTM2_CH1   PWM2
		PINS_Init(&config);

		config.mux=PORT_MUX_ALT3;
		PINS_Init(&config);
		config.base=PORTC;
		config.gpioBase=PTC;
		config.pinPortIdx=0;  //FTM0_CH0   PWM0
		PINS_Init(&config);
	
		config.mux=PORT_MUX_ALT2;
		PINS_Init(&config);
		config.base=PORTB;
		config.gpioBase=PTB;
		config.pinPortIdx=2;  //FTM1_CH0   CATCH0
		PINS_Init(&config);
		
		config.mux=PORT_MUX_ALT2;
		PINS_Init(&config);
		config.base=PORTB;
		config.gpioBase=PTB;
		config.pinPortIdx=3;  //FTM1_CH1   CATCH1
		PINS_Init(&config);		

		config.mux=PORT_MUX_ALT2;
		PINS_Init(&config);
		config.base=PORTC;
		config.gpioBase=PTC;
		config.pinPortIdx=14;  //FTM1_CH2   CATCH2
		PINS_Init(&config);
		
		config.mux=PORT_MUX_ALT2;
		PINS_Init(&config);
		config.base=PORTC;
		config.gpioBase=PTC;
		config.pinPortIdx=15;  //FTM1_CH3   CATCH3
		PINS_Init(&config);
		
	{
		ftm_user_config_t info;
		ftm_state_t state;
		ftm_pwm_param_t  param;
		ftm_pwm_fault_param_t  faultConfig;
		ftm_independent_ch_param_t  pwmIndependentChannelConfig[4];
		
		memset(&info,0,sizeof(ftm_user_config_t));
		memset(&state,0,sizeof(ftm_state_t));
		memset(&param,0,sizeof(ftm_pwm_param_t));
		memset(&faultConfig,0,sizeof(ftm_pwm_fault_param_t));
		memset(&pwmIndependentChannelConfig,0,sizeof(ftm_independent_ch_param_t));
		
		info.ftmMode=FTM_MODE_EDGE_ALIGNED_PWM;
		FTM_DRV_Init(2,&info,& state);
		FTM_DRV_Init(1,&info,& state);
		FTM_DRV_Init(0,&info,& state);
		
		param.faultConfig=&faultConfig;
		param.pwmIndependentChannelConfig=pwmIndependentChannelConfig; 
		
		param.nNumIndependentPwmChannels=2;
		//FTM2_CH0   PWM1
		pwmIndependentChannelConfig[0].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[0].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[0].hwChannelId=0;
    //FTM2_CH1   PWM2
		pwmIndependentChannelConfig[1].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[1].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[1].hwChannelId=1;

		FTM_DRV_InitPwm(2,&param );
		
		param.nNumIndependentPwmChannels=1;
    //FTM0_CH0   PWM0
		pwmIndependentChannelConfig[0].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[0].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[0].hwChannelId=0;

		FTM_DRV_InitPwm(0,&param );
/*		
		param.nNumIndependentPwmChannels=4;
		//FTM1_CH0   CATCH0
		pwmIndependentChannelConfig[0].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[0].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[0].hwChannelId=0;
    //FTM1_CH1   CATCH1
		pwmIndependentChannelConfig[1].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[1].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[1].hwChannelId=1;
		//FTM1_CH2   CATCH2
		pwmIndependentChannelConfig[2].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[2].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[2].hwChannelId=2;
    //FTM1_CH3   CATCH3
		pwmIndependentChannelConfig[3].polarity=FTM_POLARITY_HIGH;
		pwmIndependentChannelConfig[3].levelSelect=FTM_LOW_TRUE_PULSE;//设置输出高电平
		pwmIndependentChannelConfig[3].hwChannelId=3;
		
		FTM_DRV_InitPwm(1,&param );		
*/		
		FTM_DRV_InitModule(FTM0,FTM_CLOCK_DIVID_BY_8);//128分频
		FTM_DRV_InitModule(FTM1,FTM_CLOCK_DIVID_BY_8);//128分频
		FTM_DRV_InitModule(FTM2,FTM_CLOCK_DIVID_BY_8);//128分频
		

		FTM_DRV_SetModuloCounterValue(0,PWM_FREQ_20KHz,false);//设置PWM频率 8M时钟 /8分频 =1000000   
		FTM_DRV_SetModuloCounterValue(1,PWM_FREQ_20KHz,false);//设置PWM频率 8M时钟 /8分频 =1000000  
		FTM_DRV_SetModuloCounterValue(2,PWM_FREQ_20KHz,false);//设置PWM频率 8M时钟 /8分频 =1000000 


		{
			uint8_t chanl[1]={0};
			uint16_t vol[1]={0};//
			FTM_DRV_FastUpdatePwmChannels(0,1,chanl,vol,false);
		}
		
		{
			uint8_t chanl[4]={0,1,2,3};
			uint16_t vol[4]={0,0,0,0};//
			FTM_DRV_FastUpdatePwmChannels(1,4,chanl,vol,false);
		}

		{
			uint8_t chanl[2]={0,1};
			uint16_t vol[2]={PWM_FREQ_20KHz,0};//
			FTM_DRV_FastUpdatePwmChannels(2,2,chanl,vol,false);
		}
		
		 FTM2->SC |= FTM_SC_CLKS(3);//配置成功后设置时钟源
		 FTM1->SC |= FTM_SC_CLKS(3);//配置成功后设置时钟源
		 FTM0->SC |= FTM_SC_CLKS(3);//配置成功后设置时钟源
	 }
//			
		
}

void Interface_Windshield_Wiper(u8 tem) //FTM2_CH0 PWM1
{
	uint8_t chanl[1]={0};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM2)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(2,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}

void Interface_FTM2_CH1(u8 tem) //PWM2
{
	uint8_t chanl[1]={1};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM2)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(2,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}

void Interface_FTM0_CH0(u8 tem) //PWM0
{
	uint8_t chanl[1]={0};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM0)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(0,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}

void Interface_FTM1_CH0(u8 tem) //CATCH0
{
	uint8_t chanl[1]={0};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM1)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(1,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}

void Interface_FTM1_CH1(u8 tem) //CATCH1
{
	uint8_t chanl[1]={1};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM1)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(1,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}

void Interface_FTM1_CH2(u8 tem) //CATCH2
{
	uint8_t chanl[1]={2};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM1)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(1,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}

void Interface_FTM1_CH3(u8 tem) //CATCH3
{
	uint8_t chanl[1]={3};
	uint16_t vol[1];//
	vol[0]=((u32)tem*FTM_DRV_GetMod(FTM1)/100);

	printf("\r\n FTM2 Ch0 = %d",vol[0]);
	FTM2->SC &=~FTM_SC_CLKS_MASK;
	FTM_DRV_FastUpdatePwmChannels(1,1,chanl,vol,false);
	FTM2->SC |= FTM_SC_CLKS(3);	
}














