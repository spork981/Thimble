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

void setup() {
#ifdef WATCHDOG
    wdt_disable(); // just in case
    wdt_enable(WDTO_4S);
    wdt_reset();
#endif
    
    pinMode(STATLED, OUTPUT); // status LED
    input.reserve(127);

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
        Serial.print(freeMem());
        Serial.println(" bytes free");
        
        Serial.print(input);
        
        // parse gcode stuff
        gcs.pushAndParse(input);        

        GcodeInstruction gci = gcs.popBuffer();
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
        
        input = "";
    }
    
}
