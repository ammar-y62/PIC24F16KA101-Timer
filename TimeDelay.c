/* 
 * File:   TimeDelay.c
 * Author: Ammar Elzeftawy, David Kim, Yaseen ElSherif
 *
 * Created on October 10, 2022, 6:32 PM
 */

#include <stdio.h>
#include "xc.h"

#define SCALER 15

void delay_ms(uint16_t time_ms){
    PR2 = SCALER* time_ms;
    T2CONbits.TON = 1;
    Idle();
    T2CONbits.TON = 0;
    return;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF=0; //Clear timer 2 interrupt flag
    T2CONbits.TON=0; //stop timer
    TMR2 = 0;
    return;
}