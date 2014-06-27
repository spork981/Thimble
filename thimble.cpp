/* 
 * File:   thimble.cpp
 * This is the main file - Everything happens here.
 * 
 */

#include <avr/wdt.h>

#include "thimble.hpp"
#include "stepper.hpp"
#include "coordsystem.hpp"
#include "gcodeinterpreter.hpp"

void setup();
void loop();

String input = "";
GcodeStack gcs;
ShiftRegister shiftregister;
ShiftStepper   stepperx;
ShiftStepper   steppery;
BipolarStepper stepperz;

void setup() {
#ifdef WATCHDOG
    wdt_disable(); // just in case
    wdt_enable(WDTO_4S);
    wdt_reset();
#endif
    
    pinMode(STATLED, OUTPUT); // status LED
    input.reserve(127);

    shiftregister(SR_CLOCKPIN, SR_LATCHPIN, SR_DATAPIN, 1);
    stepperx(shiftregister, 0);    // there will be some sort of
    steppery(shiftregister, 1);    // preprocessor setting for these, someday
    stepperz(4, 5, 6, 7);
    
    Serial.begin(115200);
    Serial.println("ready.");
}

void loop() {
#ifdef WATCHDOG
    wdt_reset();
#endif
    
    /*
    digitalWrite(STATLED, HIGH);
    delay(500);
    digitalWrite(STATLED, LOW);
    delay(500);
     */
    
#ifdef WATCHDOG
    wdt_reset();
#endif
    
    // read the Serial port for commands
    while(Serial.available()) {
        char in = (char) Serial.read();
        input += in;
        if(in == '\n')
            break;
    }
    
    if(input.length() > 0) {
        Serial.print(freeMem()); // we don't want any stack crashes
        Serial.println(" bytes free");
        
        Serial.print(input);
        
        // parse gcode stuff
        gcs.pushAndParse(input);        

        // strictly debug stuff here - may get removed later
        GcodeInstruction gci = gcs.peekBuffer();
        Serial.print("gcode instruction: ");
        Serial.println(gci.instruction);
        Serial.print("gcode X: ");
        Serial.println(gci.argument[0]);
        Serial.print("gcode Y: ");
        Serial.println(gci.argument[1]);
        Serial.print("gcode Z: ");
        Serial.println(gci.argument[2]);
        Serial.print("gcode E: ");
        Serial.println(gci.argument[3]);     
        Serial.println();
        
        gci = gcs.popBuffer();
        switch(gci.instruction) {
            case G0:
            case G1:
                moveHead(gci.argument);
        }
        
        input = "";
    }
    
}

void moveHead(long position[3]) {
    // for now, we're assuming given Gcode is in mm
    // also, we're only moving one axis at a time - this may need some
    // redesigning to move multiple steppers at once
    stepperx.move(position[0] * XSTEP / PRECISION);
    steppery.move(position[1] * YSTEP / PRECISION);
    stepperz.move(position[2] * ZSTEP / PRECISION);
}