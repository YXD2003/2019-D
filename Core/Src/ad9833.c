
#include "main.h"
#include "ad9833.h"

#define	CS_9833_0()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)//FNC片选线
#define	CS_9833_1()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)

/*
***********************************************************************
*	函 数 名: AD9833_GPIOinit
*	功能说明: 初始化AD9833相关GPIO
*	形    参: 无
*	返 回 值: 无
************************************************************************
*/


extern SPI_HandleTypeDef hspi2;//外部声明结构体spi2

void AD9833_GPIOinit(void)
{
	CS_9833_1();
}

/*
***********************************************************************
*	函 数 名: AD9833_Write
*	功能说明: 向AD9833发送一个16bit数
*	形    参: TxData 要发送的数
*	返 回 值: 无
************************************************************************
*/
void AD9833_Write(unsigned short TxData)
{
	unsigned char data[2];
	data[0] = (unsigned char)((TxData>>8)&0xff);
	data[1] = (unsigned char)(TxData&0xff);
	CS_9833_0();
	HAL_SPI_Transmit(&hspi2, data, 2, 0x02);
	CS_9833_1();
}


/*
*********************************************************************************************************
*	函 数 名: AD9833_FreqSet
*	功能说明: 设置AD9833输出频率，设置完后暂不输出
*	形    参: Freq: 频率值, 0.1 hz - 12.5Mhz(最大值为时钟的一半)
					如需设置输出1MHz，则填入1000000
*	返 回 值: 无
*********************************************************************************************************
*/
void AD9833_FreqSet(double Freq)
{
	int frequence_LSB,frequence_MSB;
	double   frequence_mid,frequence_DATA;
	long int frequence_hex;

	//频率换算
	frequence_mid=268435456/25; //使用25M外部时钟
	frequence_DATA=Freq;
	frequence_DATA=frequence_DATA/1000000;
	frequence_DATA=frequence_DATA*frequence_mid;
	frequence_hex=frequence_DATA;  //32bit
	frequence_LSB=frequence_hex; //频率低位
	frequence_LSB=frequence_LSB&0x3fff; //14bit
	frequence_MSB=frequence_hex>>14; //频率高位
	frequence_MSB=frequence_MSB&0x3fff; //14bit

	//使用频率寄存器0输出波形
	frequence_LSB=frequence_LSB|0x4000;
	frequence_MSB=frequence_MSB|0x4000;

	AD9833_Write(0x2100); //选择数据一次写入，复位(无输出)
	AD9833_Write(frequence_LSB); //L14，选择频率寄存器0的低14位数据输入
	AD9833_Write(frequence_MSB); //H14 频率寄存器的高14位数据输入
}


/*
*********************************************************************************************************
*	函 数 名: 	AD9833_CtrlSet
*	功能说明: 	设置AD9833输出形式
*	形    参: 	Reset：0为有输出，1为无输出，此位只控制有无输出，不复位寄存器
							SleeppMode：3关闭内部时钟和DAC，0不关闭
							optionbit | modebit：
									00正弦波，01三角波，10方波，11保留
*	返 回 值: 无
*********************************************************************************************************
*/
void AD9833_CtrlSet(unsigned char Reset, unsigned char SleeppMode, 
					unsigned char optionbit, unsigned char modebit)
{
	unsigned short regtmep = 0;
	regtmep = regtmep|(((unsigned short)Reset&0x01)<<8);
	regtmep = regtmep|((SleeppMode&0x03)<<6);
	regtmep = regtmep|((optionbit&0x01)<<5);
	regtmep = regtmep|((modebit&0x01)<<1);
	
	AD9833_Write(regtmep);
	//AD9833_Write(0x2000|regtmep);
}
