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

void BipolarStepper::setDirection(bool direction) {
    this->direction = direction;
}

void BipolarStepper::step() {
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

    if(direction)
        stepcount++;
    else
        stepcount--;

    if(stepcount > 3)
        stepcount = 0;
    else if(stepcount < 0)
        stepcount = 3;
}

ShiftStepper::ShiftStepper(ShiftRegister shiftregister, int id) {
    sr = shiftregister;
    this->id = id;
    stepcount = 0;
}

void ShiftStepper::setDirection(bool direction) {
    this->direction = direction;
}

void ShiftStepper::step() {
    switch(stepcount) {
        case 0:
            // yuck - if we have 8-bit shift registers, we can't overwrite 
            // the other stepper instructions on the other 4 bits
            sr.shiftdata[id] = 0b1001;
            break;
        case 1:
            sr.shiftdata[id] = 0b1100;
            break;
        case 2:
            sr.shiftdata[id] = 0b0110;
            break;
        case 3:
            sr.shiftdata[id] = 0b0011;
            break;
    }

    // update shift registers    
    for(int i=0; i<2; i+=2) {
        int data = sr.shiftdata[i];
        data += (sr.shiftdata[i+1] << 4);
        
        DWR(sr.lthpin, LOW);
        shiftOut(sr.datpin, sr.clkpin, MSBFIRST, data);
        DWR(sr.lthpin, HIGH);
    }
    
    if(direction)
        stepcount++;
    else
        stepcount--;

    if(stepcount > 3)
        stepcount = 0;
    else if(stepcount < 0)
        stepcount = 3;

}

StepperStateMachine::StepperStateMachine(SensorSystem* sens, Stepper* x, Stepper* y, Stepper* z) {
    sensors  = sens;
    xstepper = x;
    ystepper = y;
    zstepper = z;
    xstepper_steps = 0;
    ystepper_steps = 0;
    zstepper_steps = 0;
}

void StepperStateMachine::moveX(int steps) {
    if(steps == 0)
        return;     // we don't need to do anything
    
    xstepper_last = millis();
    xstepper_steps = abs(steps) - 1;    
    // set the direction of the motor, depending on whether # of steps is positive or negative
    xstepper->setDirection((steps > 0) ? STEP_FORWARD : STEP_BACK);
    if(sensors->checkXStop() == LOW)
        xstepper->step();
}

void StepperStateMachine::moveY(int steps) {
    if(steps == 0)
        return;     // we don't need to do anything
    
    ystepper_last = millis();
    ystepper_steps = abs(steps) - 1;    
    // set the direction of the motor, depending on whether # of steps is positive or negative
    ystepper->setDirection((steps > 0) ? STEP_FORWARD : STEP_BACK);
    if(sensors->checkYStop() == LOW)
        ystepper->step();
}

void StepperStateMachine::moveZ(int steps) {
    if(steps == 0)
        return;     // we don't need to do anything
    
    zstepper_last = millis();
    zstepper_steps = abs(steps) - 1;
    // set the direction of the motor, depending on whether # of steps is positive or negative
    zstepper->setDirection((steps > 0) ? STEP_FORWARD : STEP_BACK);
    if(sensors->checkZStop() == LOW)
        zstepper->step();
}

// Note: this is a blocking action! (uses delays instead of millis())
void StepperStateMachine::home(int axes[3]) {
    // quickly move axes
    moveX(axes[0]);
    moveY(axes[1]);
    moveZ(axes[2]);
    
    while(xstepper_steps > 0 && ystepper_steps > 0 && zstepper_steps > 0) {
        delay(5);
#ifdef WATCHDOG
        wd_reset();
#endif
    }
    
    // now move them up, and back towards home slowly
    xstepper->setDirection(STEP_FORWARD);
    ystepper->setDirection(STEP_FORWARD);
    zstepper->setDirection(STEP_FORWARD);
    for(int i=0; i<50; i++) {
        if(axes[0] > 0) 
            xstepper->step();
        if(axes[1] > 0) 
            ystepper->step();
        if(axes[2] > 0) 
            zstepper->step();
        delay(5); // TODO: replace this with speed setting in hpp
    }
    
    xstepper->setDirection(STEP_BACK);
    ystepper->setDirection(STEP_BACK);
    zstepper->setDirection(STEP_BACK);
    while(sensors->checkXStop() != HIGH) {
        xstepper->step();
        delay(10);
    }
    
    while(sensors->checkYStop() != HIGH) {
        ystepper->step();
        delay(10);
    }
    
    while(sensors->checkZStop() != HIGH) {
        zstepper->step();
        delay(10);
    }
    // now we're homed!
}

int StepperStateMachine::updateSteppers() {
    byte returnstatus = 0;
    if(xstepper_steps > 0 && millis() - xstepper_last > (1000 / STEPS_PER_SECOND)) {
        xstepper_last = millis();
        xstepper_steps--;
        
        if(sensors->checkXStop() == HIGH) { // hmm, maybe check endstops only if close to home
            returnstatus = returnstatus ^ 0b1;
            xstepper_steps = 0;
        } else
            xstepper->step();
    }
    
    if(ystepper_steps > 0 && millis() - ystepper_last > (1000 / STEPS_PER_SECOND)) {
        ystepper_last = millis();
        ystepper_steps--;

        if(sensors->checkYStop() == HIGH) {
            returnstatus = returnstatus ^ 0b10;
            ystepper_steps = 0;
        } else
            ystepper->step();
    }
    
    if(zstepper_steps > 0 && millis() - zstepper_last > (1000 / STEPS_PER_SECOND)) {
        zstepper_last = millis();
        zstepper_steps--;
        
        if(sensors->checkZStop() == HIGH) {
            returnstatus = returnstatus ^ 0b100;
            zstepper_steps = 0;
        } else
            zstepper->step();
    }
    
    return returnstatus; // or nonzero if an error is found
}
