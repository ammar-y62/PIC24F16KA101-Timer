/* 
 * File:   IOs.c
 * Author: Ammar Elzeftawy, David Kim, Yaseen ElSherif
 *
 * Created on October 10, 2022, 6:32 PM
 */

#include "xc.h"
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include "TimeDelay.h"
#include <errno.h>
#include <stdio.h>
#include "IOs.h"

#define SCALER 13

// Global variables
int CNflag = 0;
int min = 0;        
int sec = 0;
int dispflag = 0;
int LEDflag = 0;
int stop = 0;  
int stop2 = 0;
int count = 0;
char print[100];

void IOinit() {
    // Set inputs on RA2, RA4, and RB4
    TRISAbits.TRISA2 = 1; // PB1
    TRISAbits.TRISA4 = 1; // PB2
    TRISBbits.TRISB4 = 1; // PB3
    
    // Set pull up resistors on RA2(CN30), RA4(CN0), and RB4(CN1)
    CNPU1bits.CN0PUE = 1;
    CNPU1bits.CN1PUE = 1;
    CNPU2bits.CN30PUE = 1;
    
    CNEN1bits.CN0IE = 1;  // enable CN on CN0
    CNEN1bits.CN1IE = 1;  // enable CN on CN1
    CNEN2bits.CN30IE = 1;  // enable CN on CN30
    
    CLKDIVbits.RCDIV0=0;
    CLKDIVbits.RCDIV1=0;
    CLKDIVbits.RCDIV2=0;
   
    TRISBbits.TRISB15 = 0;  // Set RB15 as output for REFO
    REFOCONbits.ROEN = 0; // Ref oscillator is disabled
    REFOCONbits.ROSSLP = 0; // Ref oscillator is disabled in sleep
    REFOCONbits.ROSEL = 1; // Output base clk showing clock switching
    REFOCONbits.RODIV = 0b0000;
    INTCON1bits.NSTDIS = 1;
    
    IPC1bits.T2IP = 0b011;
    T2CONbits.TCS=0;
    T2CONbits.T32=0;
    T2CONbits.TCKPS = 0b11;
    T2CONbits.TSIDL = 0;
    
    // CN Interrupt Settings
    IPC4bits.CNIP = 6; // Sets interrupt priority to 6 for CN. 7 highest 1 lowest 0 disable
    IFS1bits.CNIF = 0; // Clear CN interrupt flag
    IEC1bits.CNIE = 1; // Enable CN interrupt
    IEC0bits.T2IE = 1;
    
    TRISBbits.TRISB8 = 0; // Make GPIO RB8 as a digital output
    LATBbits.LATB8 = 0; // Make GPIO RB8 as a digital output
    
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital
}

void IOCheck() {
    count = 0;
    while((PORTAbits.RA2 == 0) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 1)) { // statement for PB1 input pressed
       LATBbits.LATB8 = 0; // turn off LED        
        delay_ms(1000); // 1 second delay 
        if (min >= 0 && min < 59){
            min++;
            sprintf(print, "%02d m : %02d s \r", min, sec);
            Disp2String(print);
        }            
    }
   
    count = 0;
    while((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 0) &&  (PORTBbits.RB4 == 1)) { // statement for PB2 input pressed
        LATBbits.LATB8 = 0; // turn off LED       
        delay_ms(1000); // 1 second delay
        if (sec >= 0 && sec < 59){
            sec++;
            sprintf(print, "%02d m : %02d s \r", min, sec);
            Disp2String(print);
        }
        else if(sec == 59){           
            if(min >= 0 && min < 59){
                min ++; 
                sec = 0;
            }
        }
    }

    count = 0;
    dispflag = 0;
    while((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 0)) { // statement for PB3 input pressed
        LATBbits.LATB8 = 0; // turn off LED       
        delay_ms(3000); // delay 3 second
        if(count < 1) {          
            count++;  
            stop = 1 - stop; 
            stop2 = 1;
        }
        else {
            min = 0;            
            sec = 0;
            if(dispflag == 0) {
                sprintf(print, "%02d m : %02d s \r", min, sec);
                Disp2String(print); 
                dispflag = 1;
            }
            stop2 = 0;
            stop = 0; // reset stop
        }
    }
    
    count = 0;
    CNflag = 0;
    LEDflag = 0;
    if(stop2 == 1 && CNflag == 0) {      
        while(stop == 1) {
            delay_ms(1500);
            if(LEDflag == 0) {
                LATBbits.LATB8 = 0; // turn off LED
                LEDflag = 1;
            }
            else {
                LATBbits.LATB8 = 1; // turn on LED
                LEDflag = 0;
            }
            
            if(min >= 0 && sec > 0) {
                sprintf(print, "%02d m : %02d s \r", min, sec);
                Disp2String(print);
            }
            if(min == 0 && sec == 0) {
                stop = 0;
                sprintf(print, "%02d m : %02d s - ALARM\r", min, sec);
                Disp2String(print);
                LATBbits.LATB8 = 1; // turn on LED
            }
            else if(min > 0) {
                if(sec == 0) {
                    if(min > 0){
                        min--;
                    }                
                    sec = 59;
                }
                else {
                    sec --;
                }
            }
            else {
                sec --;
            }
            if(CNflag == 1) {
                break;
            }
        }
    }         
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void){
    CNflag = 1;
    IFS1bits.CNIF = 0; // Clear interrupt flag   
    Nop();
    return;
}
