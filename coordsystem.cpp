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