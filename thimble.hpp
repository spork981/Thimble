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

// Maximum steps per second. If set too high, your steppers won't move and may
//   damage themselves!
#define STEPS_PER_SECOND  200

// Check each line with G-code's checksum algorithm. Commenting this out will
//   speed up the firmware, at the cost of command checking.
//#define CHECKSUM

// Steps per millimeter.
#define XSTEP 8
#define YSTEP 8
#define ZSTEP 12

// Shift register pins, if used.
#define SR_LATCHPIN 8
#define SR_DATAPIN  11
#define SR_CLOCKPIN 12

// X, Y, and Z steppers. Use ShiftStepper if it is a motor driven through a
// shift register, or BipolarStepper if the coils are driven by the Arduino.
#define XSTEPPER_TYPE ShiftStepper
#define YSTEPPER_TYPE ShiftStepper
#define ZSTEPPER_TYPE BipolarStepper

// X, Y, and Z endstop pins, as well as inverted statuses for each of them.
//  Uncomment the *STOP_INVERT definitions to _enable_ inversion (if you have
//  NC switches - leave them commented out if you have Normally Open endstops, 
//  which is probably a bad idea anyway).
#define XSTOP_PIN A0
#define YSTOP_PIN A1
#define ZSTOP_PIN A2
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

