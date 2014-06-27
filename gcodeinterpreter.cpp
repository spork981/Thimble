/* 
 * File:   gcodeinterpreter.cpp
 * functions and routines for the G-code interpreter.
 * 
 */

#include "thimble.hpp"
#include "gcodeinterpreter.hpp"

GcodeInstruction::GcodeInstruction() {
    instruction = 0;
    memset(argument, 0, sizeof(argument)); // clear the argument array
}

GcodeStack::GcodeStack() {
    line = 0;
    capacity = 30; // good for now - needs testing
    buffer = new GcodeInstruction[capacity];
    pushposition = 0;
    pop_position = 0;
}

int GcodeStack::getLine() {
    return line;
}

/* Parse the input string and add it to the buffer. */
int GcodeStack::pushAndParse(String input) {
    return pushBuffer(parseString(input));
}

/* Put a G-code object at the end of the buffer. */
int GcodeStack::pushBuffer(GcodeInstruction gcode) {
    buffer[pushposition] = gcode;
    
    pushposition++;
    if(pushposition >= capacity)
        pushposition = 0;
    
    if(pushposition == pop_position) // running out of buffer room
        return 1;
    
    return 0;
}

/* Take the first G-code object from the buffer and return it. */
GcodeInstruction GcodeStack::popBuffer() {
    GcodeInstruction gcode = buffer[pop_position];

    pop_position++;
    if(pop_position >= capacity)
        pop_position = 0;    
    
    return gcode;
}

/* Turn a string (one line of G-code) into a usable instruction */
GcodeInstruction GcodeStack::parseString(String input) {
    GcodeInstruction gcode;
 
#ifdef CHECKSUM
    int ck = 0;
    for(unsigned int i=0; input[i] != '*' && i < input.length(); i++)
        ck ^= input[i];
#endif
    
    for(unsigned int i=0; i<input.length(); i++) {
        switch(input[i]) {
            case '(':
            case ';':       // this is a Gcode comment - skip
                gcode.instruction = -1;
                return gcode;
            case 'N':       // line number
            {
                int tmpline;
                tmpline = parseCommand(i, input);  
                if(tmpline > line)
                    line = tmpline;
                break;
            }
            case 'G':       // buffered command
            {
                int command = parseCommand(i, input);
                GcodeInstruction g = parseG(command, i, input);
                return g;
            }
            case 'M':       // unbuffered command
            {
                int command = parseCommand(i, input);
                GcodeInstruction m = parseM(command, i, input);
                return m;
            }
#ifdef CHECKSUM
            case '*':
            {
                int checksum = parseCommand(i, input);
                if(checksum == ck)
                    gcode.cs_valid = true;
                else
                    gcode.cs_valid = false;

                return gcode; // nothing after checksum
            }
#endif
            case '\n':      // end of line - finish this command
                return gcode;
            case ' ':       // ignore whitespace
            default:
                break;
        }        
    }
    
    return gcode;
}

/* Parse the numbers that come after a letter (N, G, M) */
int GcodeStack::parseCommand(unsigned int &cursor, String input) {
    int cmd = 0;
    for (; cursor < input.length(); cursor++) {
        if(isdigit(input[cursor])) {
            cmd *= 10;
            cmd += input[cursor] - '0';
        } else if(input[cursor] == '-') {
            cmd = -cmd;
        } else if(input[cursor] == ' ') {
            break; // all done
        }
    }

    return cmd;
}

/* Parse G-code commands, including parameters. */
GcodeInstruction GcodeStack::parseG(int code, int cursor, String input) {
    GcodeInstruction g;
    g.instruction = code;
    
    // parse arguments for a particular code
    switch(code) {
        case G0:        // G0 and G1 are the same thing for RepRaps
        case G1:
            for(; cursor<input.length(); cursor++) {
                switch(input[cursor]) {
                    case 'X':
                    {
                        float f = atof(input.substring(cursor + 1).c_str());
                        g.argument[0] = f * PRECISION;
                        break;
                    }
                    case 'Y':
                    {
                        float f = atof(input.substring(cursor + 1).c_str());
                        g.argument[1] = f * PRECISION;
                        break;
                    }
                    case 'Z':
                    {
                        float f = atof(input.substring(cursor + 1).c_str());
                        g.argument[2] = f * PRECISION;
                        break;
                    }
                    case 'E':
                    {
                        Serial.println(input.substring(cursor).c_str());
                        float f = atof(input.substring(cursor + 1).c_str());
                        Serial.println(f);
                        g.argument[3] = f * PRECISION;
                        Serial.println(f * PRECISION);
                        Serial.println(g.argument[3]);
                        break;
                    }
                    //case 'F':    // we're ignoring feedrate for now
                }
            }            
        default:
            break;
    }
    
    return g;
}

/* Parse G-code commands starting with 'M'. These are mostly RepRap specific */
GcodeInstruction GcodeStack::parseM(int code, int cursor, String input) {
    GcodeInstruction m;
    m.instruction = code + 100;
    
    switch(code) {
        case M0:        // stop
        case M1:        // sleep
        case M112:      // emergency stop    
            return m;
    }    
    return m;
}