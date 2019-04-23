/* 
 * File:   OPT3001.h
 * Author: bmcgarvey
 *
 * Created on August 23, 2016, 1:16 PM
 */

#ifndef OPT3001_H
#define	OPT3001_H

//Pins
//SCL = RD6
//SDA = RD5
//INT = RB1

#define OPT3001_ADD     0x88
//#define OPT3001_ADD     0x8a

#define CONFIG_REG  0x01
#define RESULT_REG  0x00
#define LOW_LIMIT_REG   0x02
#define HIGH_LIMIT_REG   0x03
#define MANUFACTURER_ID_REG    0x7e
#define DEVICE_ID_REG   0x7f

#ifdef	__cplusplus
extern "C" {
#endif

    void InitOPT3001(void);
    void OPT3001WriteRegister(char reg, unsigned int value);
    int OPT3001ReadRegister(char reg);
    double OPT3001ReadLux(void);

#ifdef	__cplusplus
}
#endif

#endif	/* OPT3001_H */

