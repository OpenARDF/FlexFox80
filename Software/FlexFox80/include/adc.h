/*
 * adc.h
 *
 * Created: 11/21/2021 3:54:18 PM
 *  Author: charl
 */ 


#ifndef ADC_H_
#define ADC_H_

void ADC0_startConversions(void);
int16_t ADC0_read(void);
bool ADC0_conversionDone(void);

#endif /* ADC_H_ */