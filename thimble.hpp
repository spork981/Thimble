/* 
 * File:   thimble.hpp
 * Printer-critical classes and definitions go here, as well as generic
 * shortcuts (such as digitalWrite, etc).
 * 
 */

#ifndef THIMBLE_HPP
#define	THIMBLE_HPP

#include "arduino/Arduino.h"

/*** Thimble options (user config) ***/

// Define precision, in inverse millimeters. That is, 1 is 1 millimeter
//   whereas 100 is .01 millimeters.
#define PRECISION 1000

// Check each line with G-code's checksum algorithm. Commenting this out will
//   speed up the firmware, at the cost of command checking.
//#define CHECKSUM

// X, Y, and Z endstop pins, as well as inverted statuses for each of them.
//  Uncomment the *STOP_INVERT definitions to _enable_ inversion.
#define XSTOP 2
#define YSTOP 3
#define ZSTOP 4
//#define XSTOP_INVERT
//#define YSTOP_INVERT
//#define ZSTOP_INVERT

// Watchdog to keep everything under control if something bad happens.
//   Uncomment this out if you have an AVR with a new bootloader (such as optiboot)
//   or only if you know what you're doing. If you have an older bootloader, this
//   may throw it into a reset loop.
//#define WATCHDOG

/*** end user config ***/

// generic Arduino pins
#define STATLED 13

// these could be updated to modify registers directly
#define DWR digitalWrite
#define DRD digitalRead

int freeMem();

/* 8-bit shift register code (ignore if you aren't using any for steppers)
 * 
 * Note: due to the current design of this code, shift registers will most likely
 * be unusably slow if daisy-chained to more than 2 ICs.
 */
class ShiftRegister {
public:
    ShiftRegister(int, int, int, int);
    char* shiftdata;
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
    SensorSystem();
    bool checkXStop();
    bool checkYStop();
    bool checkZStop();    
    int  checkHotendTemp();
    int  checkBedTemp();
private:
    bool debounce();
};

#endif	/* THIMBLE_HPP */
