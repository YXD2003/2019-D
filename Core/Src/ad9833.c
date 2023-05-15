
#include "main.h"
#include "ad9833.h"

#define	CS_9833_0()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)//FNCƬѡ��
#define	CS_9833_1()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)

/*
***********************************************************************
*	�� �� ��: AD9833_GPIOinit
*	����˵��: ��ʼ��AD9833���GPIO
*	��    ��: ��
*	�� �� ֵ: ��
************************************************************************
*/


extern SPI_HandleTypeDef hspi2;//�ⲿ�����ṹ��spi2

void AD9833_GPIOinit(void)
{
	CS_9833_1();
}

/*
***********************************************************************
*	�� �� ��: AD9833_Write
*	����˵��: ��AD9833����һ��16bit��
*	��    ��: TxData Ҫ���͵���
*	�� �� ֵ: ��
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
*	�� �� ��: AD9833_FreqSet
*	����˵��: ����AD9833���Ƶ�ʣ���������ݲ����
*	��    ��: Freq: Ƶ��ֵ, 0.1 hz - 12.5Mhz(���ֵΪʱ�ӵ�һ��)
					�����������1MHz��������1000000
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD9833_FreqSet(double Freq)
{
	int frequence_LSB,frequence_MSB;
	double   frequence_mid,frequence_DATA;
	long int frequence_hex;

	//Ƶ�ʻ���
	frequence_mid=268435456/25; //ʹ��25M�ⲿʱ��
	frequence_DATA=Freq;
	frequence_DATA=frequence_DATA/1000000;
	frequence_DATA=frequence_DATA*frequence_mid;
	frequence_hex=frequence_DATA;  //32bit
	frequence_LSB=frequence_hex; //Ƶ�ʵ�λ
	frequence_LSB=frequence_LSB&0x3fff; //14bit
	frequence_MSB=frequence_hex>>14; //Ƶ�ʸ�λ
	frequence_MSB=frequence_MSB&0x3fff; //14bit

	//ʹ��Ƶ�ʼĴ���0�������
	frequence_LSB=frequence_LSB|0x4000;
	frequence_MSB=frequence_MSB|0x4000;

	AD9833_Write(0x2100); //ѡ������һ��д�룬��λ(�����)
	AD9833_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���0�ĵ�14λ��������
	AD9833_Write(frequence_MSB); //H14 Ƶ�ʼĴ����ĸ�14λ��������
}


/*
*********************************************************************************************************
*	�� �� ��: 	AD9833_CtrlSet
*	����˵��: 	����AD9833�����ʽ
*	��    ��: 	Reset��0Ϊ�������1Ϊ���������λֻ�����������������λ�Ĵ���
							SleeppMode��3�ر��ڲ�ʱ�Ӻ�DAC��0���ر�
							optionbit | modebit��
									00���Ҳ���01���ǲ���10������11����
*	�� �� ֵ: ��
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
