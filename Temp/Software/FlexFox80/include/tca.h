/*
 * tca.h
 *
 * Created: 4/17/2022 10:39:36 AM
 *  Author: charl
 */ 


#ifndef TCA_H_
#define TCA_H_

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
	#endif

	int8_t TIMERA_init();
    
    typedef void (*TCA0_cb_t)(void);

    int8_t TCA0_Initialize();
    void TCA0_SetOVFIsrCallback(TCA0_cb_t cb);
    void TCA0_SetCMP0IsrCallback(TCA0_cb_t cb);
    void TCA0_SetCMP1IsrCallback(TCA0_cb_t cb);
    void TCA0_SetCMP2IsrCallback(TCA0_cb_t cb);
    void TCA0_EnableInterrupt(void);
    void TCA0_DisableInterrupt(void);
    uint16_t TCA0_ReadTimer(void);
    void TCA0_WriteTimer(uint16_t timerVal);
    void TCA0_ClearOverflowInterruptFlag(void);
    bool TCA0_IsOverflowInterruptEnabled(void);
    void TCA0_ClearCMP0InterruptFlag(void);
    bool TCA0_IsCMP0InterruptEnabled(void);
    void TCA0_ClearCMP1InterruptFlag(void);
    bool TCA0_IsCMP1InterruptEnabled(void);
    void TCA0_ClearCMP2InterruptFlag(void);
    bool TCA0_IsCMP2InterruptEnabled(void);

	#ifdef __cplusplus
}
#endif

#endif /* TCA_H_ */