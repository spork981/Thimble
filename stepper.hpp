/* 
 * File:   stepper.hpp
 * Stepper motor information is stored here.
 * 
 * Currently there are two ways of driving stepper motors: directly (4 pins
 * driven by the Arduino) and through a shift register (4 pins off of a HC595-like
 * 8 bit shift register).
 */

#ifndef STEPPER_HPP
#define	STEPPER_HPP

#include "thimble.hpp"

class Stepper {
public:
    int move(int);
};

/* Bipolar stepper - 4 wires are controlled by the Arduino */
class BipolarStepper: public Stepper {
public:
    BipolarStepper(int, int, int, int);
    int move(int);
private:
    int pinA_1;
    int pinA_2;
    int pinB_1;
    int pinB_2;
    int stepcount;
};

/* Shift register stepper - driven by a shift register controlled by an Arduino */
class ShiftStepper: public Stepper {
public:
    ShiftStepper(ShiftRegister*, int);
    int move(int);
    
private:
    ShiftRegister* sr;
    int id;
    int stepcount;
};

/* Stepper driver motor - driven by Polulu-like drivers (enable, direction, step) */
class DriverStepper: public Stepper {
public:
    DriverStepper(int, int, int);
    int move(int);
};

#endif	/* STEPPER_HPP */

