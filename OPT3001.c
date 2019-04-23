#include <xc.h>
#include "OPT3001.h"

void InitOPT3001(void) {
    TRISDbits.TRISD5 = 1;
    TRISDbits.TRISD6 = 1;
    SSP2ADD = 0x63; //100kHz
    SSP2CON1bits.SSPM = 0b1000; //I2C Master mode
    SSP2CON1bits.SSPEN = 1; //Enable MSSP
    OPT3001WriteRegister(LOW_LIMIT_REG, 0xC000);  //End of conversion mode
    OPT3001WriteRegister(CONFIG_REG, 0xCC10); //Continuous conversion, 800ms conversion time, latched
}

void OPT3001WriteRegister(char reg, int value) {
    char data;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1);
    data = SSP2BUF; //Read SSPxBUF to make sure BF is clear
    SSP2BUF = OPT3001_ADD; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2BUF = reg; //Send register address
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2BUF = (value >> 8); //Send high byte
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2BUF = value; //low byte
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1);
}
int OPT3001ReadRegister(char reg) {
    int data;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1);
    data = SSP2BUF; //Read SSPxBUF to make sure BF is clear
    SSP2BUF = OPT3001_ADD; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2BUF = reg; //Send register address
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2CON2bits.RSEN = 1; //Restart
    while (SSP2CON2bits.RSEN == 1);
    SSP2BUF = OPT3001_ADD | 1; //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W);
    SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
    while (!SSP2STATbits.BF); // wait until byte received
    data = SSP2BUF;
    data <<= 8;
    SSP2CON2bits.ACKDT = 0; //ACK
    SSP2CON2bits.ACKEN = 1; //Send ACK
    while (SSP2CON2bits.ACKEN != 0);
    SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
    while (!SSP2STATbits.BF); // wait until byte received
    data |= SSP2BUF;
    SSP2CON2bits.ACKDT = 1; //NACK for last byte
    SSP2CON2bits.ACKEN = 1; //Send NACK
    while (SSP2CON2bits.ACKEN != 0);
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1);
    return data;
}

double OPT3001ReadLux(void) {
    double lux = 0;
    int value = OPT3001ReadRegister(RESULT_REG);
    int exp = value >> 12;
    int frac = value & 0x0fff;
    lux = 0.01 * (1 << exp) * frac;
    return lux;
}
