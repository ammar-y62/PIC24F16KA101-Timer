/* 
 * File:   TimeDelay.h
 * Author: Ammar Elzeftawy, David Kim, Yaseen ElSherif
 *
 * Created on October 10, 2022, 6:32 PM
 */

#ifndef TIMEDELAY_H
#define	TIMEDELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

void delay_ms(uint16_t time_ms);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* TIMEDELAY_H */