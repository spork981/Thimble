/* 
 * File:   stepper.cpp
 * Stepper motor code is stored here.
 * 
 */

#include "thimble.hpp"
#include "stepper.hpp"
#include "arduino/wiring_private.h"

BipolarStepper::BipolarStepper(int pin1, int pin2, int pin3, int pin4) {
    pinA_1 = pin1;
    pinA_2 = pin2;
    pinB_1 = pin3;
    pinB_2 = pin4;
    stepcount = 0;
}

int BipolarStepper::move(int step) {
    bool fwd = true; // direction
    if(step < 0) {
        fwd = false;
        step = -step;
    }
    
    for(; step>0; step--) {
        switch(stepcount) {
            case 0:
                DWR(pinA_1, HIGH);
                DWR(pinA_2, LOW);
                DWR(pinB_1, LOW);
                DWR(pinB_2, HIGH);
                break;
            case 1:
                DWR(pinA_1, HIGH);
                DWR(pinA_2, HIGH);
                DWR(pinB_1, LOW);
                DWR(pinB_2, LOW);
                break;
            case 2:
                DWR(pinA_1, LOW);
                DWR(pinA_2, HIGH);
                DWR(pinB_1, HIGH);
                DWR(pinB_2, LOW);
                break;
            case 3:
                DWR(pinA_1, LOW);
                DWR(pinA_2, LOW);
                DWR(pinB_1, HIGH);
                DWR(pinB_2, HIGH);
                break;
        }
        
        if(fwd)
            stepcount++;
        else
            stepcount--;
        
        if(stepcount > 3)
            stepcount = 0;
        else if(stepcount < 0)
            stepcount = 3;
        
        // at some point also check endstops
    }
    
    // these will need delays here later on
    
    return step; // should be 0 if nothing unexpected happened
}

ShiftStepper::ShiftStepper(ShiftRegister* shiftregister, int id) {
    sr = shiftregister;
    this->id = id;
    stepcount = 0;
}

int ShiftStepper::move(int step) {
    bool fwd = true; // direction
    if(step < 0) {
        fwd = false;
        step = -step;
    }

    for(; step>0; step--) {
        switch(stepcount) {
            case 0:
                // yuck - if we have 8-bit shift registers, we can't overwrite 
                // the other stepper instructions on the other 4 bits
                sr->shiftdata[id] = 0b1001;
                break;
            case 1:
                sr->shiftdata[id] = 0b1100;
                break;
            case 2:
                sr->shiftdata[id] = 0b0110;
                break;
            case 3:
                sr->shiftdata[id] = 0b0011;
                break;
        }
        
        // update shift registers
        for(int i=0; i<id; i++) {
            if(i % 2 == 0)
                DWR(sr->lthpin, LOW);
            
            shiftOut(sr->datpin, sr->clkpin, MSBFIRST, sr->shiftdata[i]);
            
            if(i % 2 == 0)
                DWR(sr->lthpin, HIGH);
        }
        
        if(fwd)
            stepcount++;
        else
            stepcount--;
        
        if(stepcount > 3)
            stepcount = 0;
        else if(stepcount < 0)
            stepcount = 3;
        
        // at some point also check endstops
    }
    
    // these will need delays here later on
    
    return step;
}