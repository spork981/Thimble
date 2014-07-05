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
StepperStateMachine ssm;
SensorSystem        sensors;
CoordSystem* coord = 0;

Stepper*   stepperx;
Stepper*   steppery;
Stepper* stepperz;

void setup() {
#ifdef WATCHDOG
    wdt_disable(); // just in case
    wdt_enable(WDTO_4S);
    wdt_reset();
#endif
    
    pinMode(STATLED, OUTPUT); // status LED
    pinMode(XSTOP_PIN, INPUT);
    pinMode(YSTOP_PIN, INPUT);
    pinMode(ZSTOP_PIN, INPUT);
    pinMode(SR_CLOCKPIN, OUTPUT);
    pinMode(SR_LATCHPIN, OUTPUT);
    pinMode(SR_DATAPIN, OUTPUT);
    //input.reserve(127);

    coord = new Cartesian();
    shiftregister = ShiftRegister(SR_CLOCKPIN, SR_LATCHPIN, SR_DATAPIN, 1);
    stepperx = new XSTEPPER_TYPE(XSTEPPER_OPTIONS);
    steppery = new YSTEPPER_TYPE(YSTEPPER_OPTIONS);
    stepperz = new ZSTEPPER_TYPE(ZSTEPPER_OPTIONS);
    ssm = StepperStateMachine(&sensors, stepperx, steppery, stepperz);
    
    Serial.begin(115200);
    showMem();
    Serial.println("start");
}

void loop() {
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
    
    if(input.length() > 1 && input[input.length() - 1] == '\n') {
        showMem();
        
        // parse gcode stuff
        if(gcs.pushAndParse(input) != 0) {
            Serial.println(G_ERROR);
        }

        // strictly debug stuff here - may get removed later
        GcodeInstruction gci = gcs.popBuffer();
        if(gci.instruction != G_BAD) {
            Serial.print("//gcode instruction: ");
            Serial.println(gci.instruction);
            Serial.print("//gcode X, Y, Z, E: ");
            Serial.print(gci.argument[0]);
            Serial.print(", ");
            Serial.print(gci.argument[1]);
            Serial.print(", ");
            Serial.print(gci.argument[2]);
            Serial.print(", ");
            Serial.println(gci.argument[3]);

            switch (gci.instruction) {
                case G0:
                case G1:
                    moveHead(gci.argument);
                    break;
                case G28:
                    homeHead(gci.argument);
                    break;
            }
        }
        input = "";
    }
    int status = ssm.updateSteppers();
    status = coord->checkHome(status);
    if(status != 0) {   // hardware error - we're shutting down
        Serial.println(G_ERROR);
    }

}
