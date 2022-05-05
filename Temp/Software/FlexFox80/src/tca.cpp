/*
 * tca.cpp
 *
 * Created: 4/17/2022 10:38:58 AM
 *  Author: charl
 */ 

#include <tca.h>
#include <port.h>

#define FRE_3_600MHZ 2
#define FRE_3_590MHZ 3
#define FRE_3_580MHZ 4
#define FRE_3_570MHZ 5
#define FRE_3_560MHZ 6
#define FRE_3_550MHZ 7
#define FRE_3_540MHZ 8
#define FRE_3_530MHZ 1
#define FRE_3_520MHZ 1
#define FRE_3_510MHZ 9
#define FRE_3_500MHZ 10

uint8_t g_fine_freq_adj = 0;
uint8_t g_x;

ISR(TCA0_CMP0_vect)
{
	#define div 67
	
	static uint8_t cnt = div;
	
	if(!cnt) 
	{
		for(g_x=g_fine_freq_adj; g_x; g_x--)
		{
//			__asm__("nop\n\t");
		}
// 		switch(g_fine_freq_adj)
// 		{
// 			case 50: // 3.52 kHz
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 
// 			case 40: // 3.53 kHz
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 
// 			case 30: // 3.54 kHz
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 
// 			case 20: // 3.55 kHz
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 
// 
// 			case 10: // 3.56 kHz
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 			__asm__("nop\n\t");
// 
// 			case 1:
// 			__asm__("nop\n\t");
// 			break;
// 			
// 			default: // 3.57 kHz
// 			break;
// 		}
		
		PORTA_toggle_port_level(0x01);
		cnt = div;
	}
	
	cnt--;
	
// 	PORTA_toggle_port_level(0x01);
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
}

ISR(TCA0_CMP1_vect)
{
// 	if (TCA0_CMP1_isr_cb != NULL)
// 	(*TCA0_CMP1_isr_cb)();
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm;
}

ISR(TCA0_CMP2_vect)
{
// 	if (TCA0_CMP2_isr_cb != NULL)
// 	(*TCA0_CMP2_isr_cb)();
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP2_bm;
}

ISR(TCA0_OVF_vect)
{
// 	if (TCA0_OVF_isr_cb != NULL)
// 	(*TCA0_OVF_isr_cb)();
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}



int8_t TIMERA_init()
{
    //Count
    TCA0.SINGLE.CNT = 0x00;

    //CMD NONE; LUPD disabled; DIR UP;
    TCA0.SINGLE.CTRLESET = 0x00;

    //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled;
    TCA0.SINGLE.CTRLFCLR = 0x00;

    //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled;
    TCA0.SINGLE.CTRLFSET = 0x00;

    //DBGRUN disabled;
    TCA0.SINGLE.DBGCTRL = 0x00;

    //Temporary data for 16-bit Access
    TCA0.SINGLE.TEMP = 0x00;

    //RUNSTDBY disabled; CLKSEL DIV1; ENABLE enabled;
//	TCA0.SINGLE.CTRLA = 0x01;

//#define USE_TCA0_FRE_GENERATOR

#ifdef USE_TCA0_FRE_GENERATOR
    //Compare 0
    TCA0.SINGLE.CMP0 = 0x0000; // Sets frequency on WOn
	// WGMode = FRQ; 
	TCA0.SINGLE.CTRLB = 0x11;
#else
    //Period
    TCA0.SINGLE.PER = 0x0000;
    //CMP2 disabled; CMP1 disabled; CMP0 enabled; OVF disabled;
    TCA0.SINGLE.INTCTRL = 0x10;
	//CMP2EN disabled; CMP1EN disabled; CMP0EN disabled; ALUPD disabled; WGMODE NORMAL; 
	TCA0.SINGLE.CTRLB = 0x0;
    //Clear all interrupt flags
    TCA0.SINGLE.INTFLAGS = 0x71;
	CPUINT.LVL1VEC = 9;
#endif
	
    //CMP2OV disabled; CMP1OV disabled; CMP0OV disabled;
    TCA0.SINGLE.CTRLC = 0x00;

    //SPLITM disabled;
    TCA0.SINGLE.CTRLD = 0x00;

    //CMD NONE; LUPD disabled; DIR disabled;
    TCA0.SINGLE.CTRLECLR = 0x00;

	//EVACTB UPDOWN; CNTBEI disabled; EVACTA CNT_POSEDGE; CNTAEI disabled;
//	TCA0.SINGLE.EVCTRL = 0x60;
	
	// PD0 is WO0 output
	PORTA_set_pin_dir(0, PORT_DIR_OUT);

    return 0;
}

// void TCA0_DefaultCMP0IsrCallback(void);
// void (*TCA0_CMP0_isr_cb)(void) = &TCA0_DefaultCMP0IsrCallback;
// void TCA0_DefaultCMP1IsrCallback(void);
// void (*TCA0_CMP1_isr_cb)(void) = &TCA0_DefaultCMP1IsrCallback;
// void TCA0_DefaultCMP2IsrCallback(void);
// void (*TCA0_CMP2_isr_cb)(void) = &TCA0_DefaultCMP2IsrCallback;
// void TCA0_DefaultOVFIsrCallback(void);
// void (*TCA0_OVF_isr_cb)(void) = &TCA0_DefaultOVFIsrCallback;

// void TCA0_DefaultCMP0IsrCallback(void)
// {
//     //Add your ISR code here
// }
// 
// void TCA0_DefaultCMP1IsrCallback(void)
// {
//     //Add your ISR code here
// }
// 
// void TCA0_DefaultCMP2IsrCallback(void)
// {
//     //Add your ISR code here
// }
// 
// void TCA0_DefaultOVFIsrCallback(void)
// {
//     //Add your ISR code here
// }
// 
// void TCA0_SetOVFIsrCallback(TCA0_cb_t cb)
// {
//     TCA0_OVF_isr_cb = cb;
// }
// 
// void TCA0_SetCMP0IsrCallback(TCA0_cb_t cb)
// {
//     TCA0_CMP0_isr_cb = cb;
// }
// 
// void TCA0_SetCMP1IsrCallback(TCA0_cb_t cb)
// {
//     TCA0_CMP1_isr_cb = cb;
// }
// 
// void TCA0_SetCMP2IsrCallback(TCA0_cb_t cb)
// {
//     TCA0_CMP2_isr_cb = cb;
// }
// 
// ISR(TCA0_CMP0_vect)
// {
//     if (TCA0_CMP0_isr_cb != NULL)
//         (*TCA0_CMP0_isr_cb)();
//     
//     TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
// }
// 
// ISR(TCA0_CMP1_vect)
// {
//     if (TCA0_CMP1_isr_cb != NULL)
//         (*TCA0_CMP1_isr_cb)();
//     
//     TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP1_bm;
// }
// 
// ISR(TCA0_CMP2_vect)
// {
//     if (TCA0_CMP2_isr_cb != NULL)
//         (*TCA0_CMP2_isr_cb)();
//     
//     TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP2_bm;
// }
// 
// ISR(TCA0_OVF_vect)
// {
//     if (TCA0_OVF_isr_cb != NULL)
//         (*TCA0_OVF_isr_cb)();
//     
//     TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
// }
// 
// 
// /**
//  * \brief Initialize TCA interface
//  */
// int8_t TCA0_Initialize()
// {
//     //Compare 0
//     TCA0.SINGLE.CMP0 = 0x01;
// 
//     //Compare 1
//     TCA0.SINGLE.CMP1 = 0x01;
// 
//     //Compare 2
//     TCA0.SINGLE.CMP2 = 0x01;
// 
//     //Count
//     TCA0.SINGLE.CNT = 0x00;
// 
//     //CMP2EN disabled; CMP1EN disabled; CMP0EN disabled; ALUPD disabled; WGMODE NORMAL; 
//     TCA0.SINGLE.CTRLB = 0x00;
// 
//     //CMP2OV disabled; CMP1OV disabled; CMP0OV disabled; 
//     TCA0.SINGLE.CTRLC = 0x00;
// 
//     //SPLITM disabled; 
//     TCA0.SINGLE.CTRLD = 0x00;
// 
//     //CMD NONE; LUPD disabled; DIR disabled; 
//     TCA0.SINGLE.CTRLECLR = 0x00;
// 
//     //CMD NONE; LUPD disabled; DIR UP; 
//     TCA0.SINGLE.CTRLESET = 0x00;
// 
//     //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled; 
//     TCA0.SINGLE.CTRLFCLR = 0x00;
// 
//     //CMP2BV disabled; CMP1BV disabled; CMP0BV disabled; PERBV disabled; 
//     TCA0.SINGLE.CTRLFSET = 0x00;
// 
//     //DBGRUN disabled; 
//     TCA0.SINGLE.DBGCTRL = 0x00;
// 
//     //EVACTB UPDOWN; CNTBEI disabled; EVACTA CNT_POSEDGE; CNTAEI disabled; 
//     TCA0.SINGLE.EVCTRL = 0x60;
// 
//     //CMP2 disabled; CMP1 disabled; CMP0 disabled; OVF enabled; 
//     TCA0.SINGLE.INTCTRL = 0x01;
// 
//     //CMP2 disabled; CMP1 disabled; CMP0 disabled; OVF disabled; 
//     TCA0.SINGLE.INTFLAGS = 0x00;
// 
//     //Period
//     TCA0.SINGLE.PER = 0xCB6;
// 
//     //Temporary data for 16-bit Access
//     TCA0.SINGLE.TEMP = 0x00;
// 
//     //RUNSTDBY disabled; CLKSEL DIV256; ENABLE enabled; 
//     TCA0.SINGLE.CTRLA = 0x0D;
// 
//     return 0;
// }
// 
// void TCA0_WriteTimer(uint16_t timerVal)
// {
//     TCA0.SINGLE.CNT=timerVal;
// }
// 
// uint16_t TCA0_ReadTimer(void)
// {
//     uint16_t readVal;
// 
//     readVal = TCA0.SINGLE.CNT;
// 
//     return readVal;
// }
// 
// void TCA0_EnableInterrupt(void)
// {
//         TCA0.SINGLE.INTCTRL = 1 << TCA_SINGLE_CMP0_bp /* Compare 0 Interrupt: enabled */
// 	 				| 1 << TCA_SINGLE_CMP1_bp /* Compare 1 Interrupt: enabled */
// 	 				| 1 << TCA_SINGLE_CMP2_bp /* Compare 2 Interrupt: enabled */
// 	 				| 1 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: enabled */
// }
// void TCA0_DisableInterrupt(void)
// {
//     TCA0.SINGLE.INTCTRL = 0 << TCA_SINGLE_CMP0_bp /* Compare 0 Interrupt: disabled */
// 	 				| 0 << TCA_SINGLE_CMP1_bp /* Compare 1 Interrupt: disabled */
// 	 				| 0 << TCA_SINGLE_CMP2_bp /* Compare 2 Interrupt: disabled */
// 	 				| 0 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: disabled */
// }
// void TCA0_ClearOverflowInterruptFlag(void)
// {
//     TCA0.SINGLE.INTFLAGS &= ~TCA_SINGLE_OVF_bm; /* Overflow Interrupt: disabled */
// }
// bool TCA0_IsOverflowInterruptEnabled(void)
// {
//     return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm) > 0);
// }
// 
// void TCA0_ClearCMP0InterruptFlag(void)
// {
//     TCA0.SINGLE.INTFLAGS &= ~TCA_SINGLE_CMP0_bm; /* Compare Channel-0 Interrupt: disabled */
// }
// 
// bool TCA0_IsCMP0InterruptEnabled(void)
// {
//     return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_CMP0_bm) > 0);
// }
// 
// void TCA0_ClearCMP1InterruptFlag(void)
// {
//     TCA0.SINGLE.INTFLAGS &= ~TCA_SINGLE_CMP1_bm; /* Compare Channel-1 Interrupt: disabled */
// }
// 
// bool TCA0_IsCMP1InterruptEnabled(void)
// {
//     return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_CMP1_bm) > 0);
// }
// 
// void TCA0_ClearCMP2InterruptFlag(void)
// {
//     TCA0.SINGLE.INTFLAGS &= ~TCA_SINGLE_CMP2_bm; /* Compare Channel-2 Interrupt: disabled */
// }
// 
// bool TCA0_IsCMP2InterruptEnabled(void)
// {
//     return ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_CMP2_bm) > 0);
// }
