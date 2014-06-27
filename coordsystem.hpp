/* 
 * File:   coordsystem.hpp
 * Coordinate system information.
 * 
 */

#ifndef COORDSYSTEM_HPP
#define	COORDSYSTEM_HPP

#include "stepper.hpp"

// Change this line to use a different coordinate system.
#define COORDSYSTEM Cartesian

class CoordSystem {
public:
    virtual int moveX(int step) = 0;
    virtual int moveY(int step) = 0;
    virtual int moveZ(int step) = 0;
    
protected:
    int distanceX, distanceY, distanceZ;
    int positionX, positionY, positionZ;
};

/* Cartesian coordinate system - default */
class Cartesian: public CoordSystem {
public:
    Cartesian();
    int moveX(int step);
    int moveY(int step);
    int moveZ(int step);
    
private:
    Stepper stepperX, stepperY, stepperZ;
};

/* Add other coordinate systems for strange printers (SCARA, delta, etc.) here. */



#endif	/* COORDSYSTEM_HPP */

