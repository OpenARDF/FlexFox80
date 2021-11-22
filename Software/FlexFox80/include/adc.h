/*
 * adc.h
 *
 * Created: 11/21/2021 3:54:18 PM
 *  Author: charl
 */ 


#ifndef ADC_H_
#define ADC_H_

typedef enum {
	ADCDisable,
	ADCAudioInput,
	ADCExternalBatteryVoltage,
	ADC12VRegulatedVoltage,
	ADCTXAdjustableVoltage,
	ADCShutdown
} ADC_Active_Channel_t;

void ADC0_setADCChannel(ADC_Active_Channel_t chan);
void ADC0_startConversion(void);
bool ADC0_conversionDone(void);
int ADC0_read(void);

#endif /* ADC_H_ */