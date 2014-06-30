/* 
 * File:   coordsystem.cpp
 * Author: daniale
 * 
 * Created on June 25, 2014, 4:09 PM
 */

#include "coordsystem.hpp"
#include "stepper.hpp"

/* Cartesian coordinate declarations */
Cartesian::Cartesian() {
    // we'll fix these later when we home axes
    distanceX = 0;
    distanceY = 0;
    distanceZ = 0;
}

/* If this were a more complex coordinate system, funny math stuff would happen here */
int Cartesian::moveX(int step) {
    positionX += step;
    return step;
}

int Cartesian::moveY(int step) {
    positionY += step;
    return step;
}

int Cartesian::moveZ(int step) {
    positionZ += step;
    return step;
}

int Cartesian::homeX() {
    long move = -positionX;
    positionX = 0;
    return move;
}

int Cartesian::homeY() {
    long move = -positionY;
    positionY = 0;
    return move;
}

int Cartesian::homeZ() {
    long move = -positionZ;
    positionZ = 0;
    return move;
}

/* If an axis has hit its endstop prematurely, update axis position and throw a warning */
int Cartesian::checkHome(int status) {
    if(status == 0) // no errors
        return 0;
    
    if(status & 0b1 == 1) // x axis
        positionX = 0;
    
    if(status & 0b10 == 1) // y axis
        positionY = 0;
    
    if(status & 0b100 == 1) // z axis
        positionZ = 0;    
    
    return 1;
}