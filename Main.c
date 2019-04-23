#include <xc.h>
#include <stdio.h>
#include "LCD.h"
#include"OPT3001.h"


#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF


void InitPins(void);
void ConfigInterrupts(void);

#define _XTAL_FREQ 32000000L

char lcdstr[17];
volatile char update;
volatile double lux;

void main(void) {
    update = 0;
    OSCTUNEbits.PLLEN = 1;
    InitPins();
    LCDInit();
    lprintf(0, "OPT3001");
    ConfigInterrupts();
    InitOPT3001();
    while (1) {
        while (!update);  //Wait for new lux value
        lprintf(1, "Lux=%.1f", lux);
        update = 0;
    }
}

void InitPins(void) {
    LATD = 0;
    TRISD = 0;
    WDTCONbits.ADSHR = 1;
    ANCON0 = ANCON1 = 0xff;  //All digital
    WDTCONbits.ADSHR = 1;
}

void ConfigInterrupts(void) {

    RCONbits.IPEN = 0; //no priorities.  This is the default.
    //set up INT1 to interrupt on falling edge
    INTCON2bits.INTEDG1 = 0; //interrupt on falling edge
    INTCON3bits.INT1IP = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1E = 1;

    INTCONbits.GIE = 1; //Turn on interrupts
}



void __interrupt(high_priority) HighIsr(void) {
    
    //Check the source of the interrupt
    if (INTCON3bits.INT1IF == 1) {
        lux = OPT3001ReadLux();
        update = 1;
        OPT3001ReadRegister(CONFIG_REG);  //Clear the OPT3001 internal interrupt flags 
        INTCON3bits.INT1IF = 0;
    }
}


