/* 
 * File:   thimble.hpp
 * Printer-critical classes and definitions go here, as well as generic
 * shortcuts (such as digitalWrite, etc).
 * 
 */

#ifndef THIMBLE_HPP
#define	THIMBLE_HPP

#include "arduino/Arduino.h"
#include "thimble.config"

// generic Arduino pins
#define STATLED 13

// these could be updated to modify registers directly
#define DWR digitalWrite
#define DRD digitalRead

int freeMem();
void showMem();

void moveHead(long position[4]);
void homeHead(long position[3]);

/* 8-bit shift register code (ignore if you aren't using any for steppers)
 * 
 * Note: due to the current design of this code, shift registers will most likely
 * be unusably slow if daisy-chained to more than 2 ICs.
 */
class ShiftRegister {
public:
    ShiftRegister();
    ShiftRegister(int, int, int, int);
    byte* shiftdata;
    int clkpin;
    int lthpin;
    int datpin;
};

/* Sensor system for the printer. This includes endstops, thermistors, and other
 *   (possible) sensors that monitor machine state. Note that this does not include
 *   input systems for UI elements (buttons, joysticks etc).
 *  
 */
class SensorSystem {
public:
    SensorSystem() { };
    bool checkXStop();
    bool checkYStop();
    bool checkZStop();    
    int  checkHotendTemp();
    int  checkBedTemp();
private:
    bool debounce();
};

#endif	/* THIMBLE_HPP */

