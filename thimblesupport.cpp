/* 
 * File:   thimblesupport.cpp
 * Printer-critical code goes here, as well as generic
 * shortcuts (such as digitalWrite, etc).
 * 
 * Basically, support functions for the classes in thimble.hpp
 * 
 */

#include "thimble.hpp"

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