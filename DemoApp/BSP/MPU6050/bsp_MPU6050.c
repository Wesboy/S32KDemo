//****************************************
// Update to MPU6050 by shinetop
// MCU: STC89C52
// 2012.3.1
// 功能: 显示加速度计和陀螺仪的10位原始数据
//****************************************
// 使用单片机STC89C52
// 晶振：11.0592M
// 显示：串口
// 编译环境 Keil uVision2
//****************************************
#include "bsp_MPU6050.h"
// #include "task.h"
#include "interface.h"
#include "Simulation_i2c.h"
#include "math.h"

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
//**************************************************************************************************
//定义类型及变量
//**************************************************************************************************
uint8_t dis[6];					//显示数字(-511至512)的字符数组
int dis_data;					//变量
int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
float aRes, gRes; // scale resolutions per LSB for the sensors
float ax, ay, az; // Stores the real accel value in g's
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer


// parameters for 6 DoF sensor fusion calculations
const float PI = 3.14159265358979323846f;                       
//********************************************************************************
//整数转字符串
//********************************************************************************
static void lcd_printf(uint8_t *s,int temp_data)
{
	if(temp_data<0)
	{
		temp_data=-temp_data;
		*s='-';
	}
	else *s=' ';

	*++s =temp_data/10000+0x30;
	temp_data=temp_data%10000;     //取余运算

	*++s =temp_data/1000+0x30;
	temp_data=temp_data%1000;     //取余运算

	*++s =temp_data/100+0x30;
	temp_data=temp_data%100;     //取余运算
	*++s =temp_data/10+0x30;
	temp_data=temp_data%10;      //取余运算
	*++s =temp_data+0x30; 	
}

/**
  * @brief   写一个字节到I2C Device中
  * @param   
  *		@arg WriteAddr:写地址 
  *		@arg iCmd:写入指令
  * @retval  无
  */
uint32_t Single_WriteI2C(uint8_t WriteAddr, uint8_t iCmd)
{
	uint8_t buf[2];

	buf[0] = WriteAddr;
	buf[1] = iCmd;

	// Interface_I2C_Write(MPU6050_ADDRESS, buf, 2);
	Simulation_I2C_WriteBytes(MPU6050_ADDRESS, WriteAddr, iCmd, 1);
	return 1;
}

/**
  * @brief   
  * @param   
  *		@arg pBuffer:存放读取的数据的缓冲区指针
  *		@arg ReadAddr:要读取数据的寄存器地址
  * @retval  无
  */
uint32_t Single_ReadI2C(uint8_t *pBuffer, uint8_t ReadAddr)
{
	bool bStatus;

	// bStatus = Interface_I2C_SlaveAddr_Read(MPU6050_ADDRESS, ReadAddr, pBuffer, 1);
	// printf("\r\nSingle_ReadI2C Status:%d\r\n", bStatus);

	Simulation_I2C_ReadBytes(MPU6050_ADDRESS, ReadAddr, pBuffer, 1);
	return 1;
}

//******************************************************************************************************
//初始化MPU6050
//******************************************************************************************************
void InitMPU6050(void)
{

	float SelfTest[6];
	uint8_t whoami = 0;
	uint8_t c;

	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);

	Single_WriteI2C(PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
	
	// get stable time source
	Single_WriteI2C(PWR_MGMT_1, 0x01); // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

	// Configure Gyro and Accelerometer
	// Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
	// DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
	// Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
	Single_WriteI2C(CONFIG, 0x03);

	// Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	Single_WriteI2C(SMPLRT_DIV, 0x04); // Use a 200 Hz rate; the same rate set in CONFIG above

	// Set gyroscope full scale range
	// Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
	Single_ReadI2C(&c, GYRO_CONFIG);
	Single_WriteI2C(GYRO_CONFIG, c & ~0xE0);		  // Clear self-test bits [7:5]
	Single_WriteI2C(GYRO_CONFIG, c & ~0x18);		  // Clear AFS bits [4:3]
	Single_WriteI2C(GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro

	// Set accelerometer configuration
	Single_ReadI2C(&c, ACCEL_CONFIG);
	Single_WriteI2C(ACCEL_CONFIG, c & ~0xE0);	   // Clear self-test bits [7:5]
	Single_WriteI2C(ACCEL_CONFIG, c & ~0x18);	   // Clear AFS bits [4:3]
	Single_WriteI2C(ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer

	Single_ReadI2C(&whoami, WHO_AM_I_MPU6050); // Read WHO_AM_I register for MPU-6050
	printf("I AM 0x%x\n\r", whoami);
	printf("I SHOULD BE 0x68\n\r");

	if (whoami == 0x68) // WHO_AM_I should always be 0x68
	{
		printf("MPU6050 is online...");
	}
	else
	{
		printf("Could not connect to MPU6050: \n\r");
		printf("%#x \n", whoami);
	}

	MPU6050SelfTest(SelfTest);
	printf("x-axis self test: acceleration trim within : ");
	printf("%f", SelfTest[0]);
	printf(" of factory value \n\r");
	printf("y-axis self test: acceleration trim within : ");
	printf("%f", SelfTest[1]);
	printf(" of factory value \n\r");
	printf("z-axis self test: acceleration trim within : ");
	printf("%f", SelfTest[2]);
	printf(" of factory value \n\r");
	printf("x-axis self test: gyration trim within : ");
	printf("%f", SelfTest[3]);
	printf(" of factory value \n\r");
	printf("y-axis self test: gyration trim within : ");
	printf("%f", SelfTest[4]);
	printf(" of factory value \n\r");
	printf("z-axis self test: gyration trim within : ");
	printf("%f", SelfTest[5]);
	printf(" of factory value \n\r");
}
//******************************************************************************************************
//合成数据
//******************************************************************************************************
static int GetData(uint8_t REG_Address)
{
	uint8_t H,L;
	Single_ReadI2C(&H, REG_Address);
	Single_ReadI2C(&L, REG_Address + 1);
	return ((H<<8)+L);   //合成数据
}
//******************************************************************************************************
//超级终端（串口调试助手）上显示10位数据
//******************************************************************************************************
void Display10BitData(int value)
{  uint8_t i;
//	value/=64;							//转换为10位数据
	lcd_printf(dis, value);			//转换数据显示
	printf("[");
	for(i=0;i<6;i++)
	{
		printf("%d", dis[i]);
	}
	printf("]");

	// 	DisplayListChar(x,y,dis,4);	//启始列，行，显示数组，显示长度
}

void getGres()
{
	switch (Gscale)
	{
	// Possible gyro scales (and their register bit settings) are:
	// 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
	// Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
	case GFS_250DPS:
		gRes = 250.0 / 32768.0;
		break;
	case GFS_500DPS:
		gRes = 500.0 / 32768.0;
		break;
	case GFS_1000DPS:
		gRes = 1000.0 / 32768.0;
		break;
	case GFS_2000DPS:
		gRes = 2000.0 / 32768.0;
		break;
	}
}

void getAres()
{
	switch (Ascale)
	{
	// Possible accelerometer scales (and their register bit settings) are:
	// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
	// Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
	case AFS_2G:
		aRes = 2.0 / 32768.0;
		break;
	case AFS_4G:
		aRes = 4.0 / 32768.0;
		break;
	case AFS_8G:
		aRes = 8.0 / 32768.0;
		break;
	case AFS_16G:
		aRes = 16.0 / 32768.0;
		break;
	}
}
void readAccelData(int16_t *destination)
{												 // x/y/z accel register data stored here
	destination[0] = GetData(ACCEL_XOUT_H);								 // Read the six raw data registers into data array
	destination[1] = GetData(ACCEL_YOUT_H);
	destination[2] = GetData(ACCEL_XOUT_H);
}

void readGyroData(int16_t *destination)
{								 				// x/y/z gyro register data stored here
	destination[0] = GetData(GYRO_XOUT_H);								 // Read the six raw data registers into data array
	destination[1] = GetData(GYRO_YOUT_H);
	destination[2] = GetData(GYRO_ZOUT_H);
}
//*******************************************************************************************************
//主程序
//*******************************************************************************************************
void MPU6050_Proc(void)
{
	int16_t accelCount[3]; // Stores the 16-bit signed accelerometer sensor output
#if 0
	printf("[    X轴加速度][   Y轴加速度][   Z轴加速度][   X轴角速度][   Y轴角速度][   Z轴角速度]\r\n");
	Display10BitData(GetData(ACCEL_XOUT_H));	//显示X轴加速度
	Display10BitData(GetData(ACCEL_YOUT_H));	//显示Y轴加速度
	Display10BitData(GetData(ACCEL_ZOUT_H));	//显示Z轴加速度
	Display10BitData(GetData(GYRO_XOUT_H));		//显示X轴角速度
	Display10BitData(GetData(GYRO_YOUT_H));		//显示Y轴角速度
	Display10BitData(GetData(GYRO_ZOUT_H));		//显示Z轴角速度
	Display10BitData(GetData(TEMP_OUT_H));		//显示Z轴角速度

	printf("\r\n");
#else
	readAccelData(accelCount);						 // Read the x/y/z adc values    // Now we'll calculate the accleration value into actual g's
	getAres();
	ax = (float)accelCount[0] * aRes;// - accelBias[0]; // get actual g value, this depends on scale being set
	ay = (float)accelCount[1] * aRes;// - accelBias[1];
	az = (float)accelCount[2] * aRes;// - accelBias[2];

	printf("accel x : %f y : %f z : %f\r\n", ax, ay, az);
#endif
}

//*******************************************************************************************************
//MPU6050SelfTest
//*******************************************************************************************************
void MPU6050SelfTest(float *destination) // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
	uint8_t rawData[4] = {0, 0, 0, 0};
	uint8_t selfTest[6];
	float factoryTrim[6];

	// Configure the accelerometer for self-test
	Single_WriteI2C(ACCEL_CONFIG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
	Single_WriteI2C(GYRO_CONFIG, 0xE0);  // Enable self test on all three axes and set gyro range to +/- 250 degrees/s

	Delayms(250);
	Single_ReadI2C(&rawData[0], SELF_TEST_X);	  // X-axis self-test results
	Single_ReadI2C(&rawData[1], SELF_TEST_Y);	// Y-axis self-test results
	Single_ReadI2C(&rawData[2], SELF_TEST_Z);  // Z-axis self-test results
	Single_ReadI2C(&rawData[3], SELF_TEST_A);  // Mixed-axis self-test results
	// Extract the acceleration test results first
	selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4; // XA_TEST result is a five-bit unsigned integer
	selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 2; // YA_TEST result is a five-bit unsigned integer
	selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 0; // ZA_TEST result is a five-bit unsigned integer
	// Extract the gyration test results first
	selfTest[3] = rawData[0] & 0x1F; // XG_TEST result is a five-bit unsigned integer
	selfTest[4] = rawData[1] & 0x1F; // YG_TEST result is a five-bit unsigned integer
	selfTest[5] = rawData[2] & 0x1F; // ZG_TEST result is a five-bit unsigned integer
	// Process results to allow final comparison with factory set values
	factoryTrim[0] = (4096.0f * 0.34f) * (pow((0.92f / 0.34f), ((selfTest[0] - 1.0f) / 30.0f))); // FT[Xa] factory trim calculation
	factoryTrim[1] = (4096.0f * 0.34f) * (pow((0.92f / 0.34f), ((selfTest[1] - 1.0f) / 30.0f))); // FT[Ya] factory trim calculation
	factoryTrim[2] = (4096.0f * 0.34f) * (pow((0.92f / 0.34f), ((selfTest[2] - 1.0f) / 30.0f))); // FT[Za] factory trim calculation
	factoryTrim[3] = (25.0f * 131.0f) * (pow(1.046f, (selfTest[3] - 1.0f)));					 // FT[Xg] factory trim calculation
	factoryTrim[4] = (-25.0f * 131.0f) * (pow(1.046f, (selfTest[4] - 1.0f)));					 // FT[Yg] factory trim calculation
	factoryTrim[5] = (25.0f * 131.0f) * (pow(1.046f, (selfTest[5] - 1.0f)));					 // FT[Zg] factory trim calculation

	//  Output self-test results and factory trim calculation if desired
	//  Serial.println(selfTest[0]); Serial.println(selfTest[1]); Serial.println(selfTest[2]);
	//  Serial.println(selfTest[3]); Serial.println(selfTest[4]); Serial.println(selfTest[5]);
	//  Serial.println(factoryTrim[0]); Serial.println(factoryTrim[1]); Serial.println(factoryTrim[2]);
	//  Serial.println(factoryTrim[3]); Serial.println(factoryTrim[4]); Serial.println(factoryTrim[5]);

	// Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
	// To get to percent, must multiply by 100 and subtract result from 100
	for (int i = 0; i < 6; i++)
	{
		destination[i] = 100.0f + 100.0f * (selfTest[i] - factoryTrim[i]) / factoryTrim[i]; // Report percent differences
	}
}
