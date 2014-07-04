/* 
 * File:   thimblesupport.cpp
 * Printer-critical code goes here, as well as generic
 * shortcuts (such as digitalWrite, etc).
 * 
 * Basically, support functions for the classes in thimble.hpp
 * 
 */

#include "thimble.hpp"
#include "stepper.hpp"
#include "coordsystem.hpp"

extern StepperStateMachine ssm;
extern CoordSystem* coord;

ShiftRegister::ShiftRegister() {
}

ShiftRegister::ShiftRegister(int clockpin, int latchpin, int datapin, int numchips) {
    clkpin = clockpin;
    lthpin = latchpin;
    datpin = datapin;
    shiftdata = new char[numchips];
}

// from https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
int freeMem() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void showMem() {
    Serial.print("//");
    Serial.print(freeMem());
    Serial.println(" bytes free");
}

void moveHead(long position[4]) {
    // for now, we're assuming given Gcode is in mm    
    ssm.moveX(coord->moveX(position[0] * XSTEP / PRECISION));
    ssm.moveY(coord->moveY(position[1] * YSTEP / PRECISION));
    ssm.moveZ(coord->moveZ(position[2] * ZSTEP / PRECISION));
}

void homeHead(long position[3]) {
    int newposition[3] = {
        (int) coord->homeX() * position[0], 
        (int) coord->homeY() * position[1],
        (int) coord->homeZ() * position[2]
    };
    ssm.home(newposition);
}