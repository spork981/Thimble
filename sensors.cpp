/* 
 * File:   sensors.cpp
 * Sensor functions and routines. These are referenced in thimble.hpp to
 * facilitate easy access to the printer's sensor systems.
 * 
 */

#include "thimble.hpp"

// Note: These functions return HIGH if they have been activated.
bool SensorSystem::checkXStop() {
    // may need debouncing code later on
    int reading = DRD(XSTOP_PIN);
#ifndef XSTOP_INVERT
    reading = !reading;
#endif    
    return reading;
}

bool SensorSystem::checkYStop() {
    // may need debouncing code later on
    int reading = DRD(YSTOP_PIN);
#ifndef YSTOP_INVERT
    reading = !reading;
#endif    
    return reading;
}

bool SensorSystem::checkZStop() {
    // may need debouncing code later on
    int reading = DRD(ZSTOP_PIN);
#ifndef ZSTOP_INVERT
    reading = !reading;
#endif    
    return reading;
}