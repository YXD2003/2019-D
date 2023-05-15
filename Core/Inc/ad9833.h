
#ifndef __AD9833_H
#define __AD9833_H 


void AD9833_FreqSet(double Freq);
void AD9833_CtrlSet(unsigned char Reset, unsigned char SleeppMode, 
					unsigned char optionbit, unsigned char modebit);
void AD9833_GPIOinit(void);


#endif 






