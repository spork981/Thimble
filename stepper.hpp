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

#define STEP_FORWARD    1
#define STEP_BACK       0

/* Generic stepper class */
class Stepper {
public:
    virtual void step() = 0;
    // virtual void microstep() = 0;     // future feature
    virtual void setDirection(bool) = 0;
private:
    bool direction;
};

/* Bipolar stepper - 4 wires are controlled by the Arduino */
class BipolarStepper: public Stepper {
public:
    BipolarStepper();
    BipolarStepper(int, int, int, int);
    void step();
    void setDirection(bool);
private:
    int pinA_1;
    int pinA_2;
    int pinB_1;
    int pinB_2;
    int stepcount;
    bool direction;
};

/* Shift register stepper - driven by a shift register controlled by an Arduino */
class ShiftStepper: public Stepper {
public:
    ShiftStepper();
    ShiftStepper(ShiftRegister, int);
    void step();
    void setDirection(bool);
private:
    ShiftRegister sr;
    int id;
    int stepcount;
    bool direction;
};

/* Stepper driver motor - driven by Polulu-like drivers (enable, direction, step) */
class DriverStepper: public Stepper {
public:
    DriverStepper();
    DriverStepper(int, int, int);
    void step();
};

/* Have a way to move stepper motors without hanging everything else up */
class StepperStateMachine {
public:
    StepperStateMachine() { };
    StepperStateMachine(Stepper*, Stepper*, Stepper*);
    int updateSteppers();
    void moveX(int);
    void moveY(int);
    void moveZ(int);
    //moveE(int);
    int getX();
    int getY();
    int getZ();
private:
    Stepper *xstepper, *ystepper, *zstepper;
    unsigned long xstepper_last, ystepper_last, zstepper_last;      // last time the 
                                                                    // steppers stepped
    int xstepper_steps, ystepper_steps, zstepper_steps;             // number of steps left
};

#endif	/* STEPPER_HPP */

