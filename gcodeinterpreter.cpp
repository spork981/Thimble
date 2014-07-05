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
    if(gcode.instruction < 0) // comment or invalid code
        return 0;
    
    buffer[pushposition] = new GcodeInstruction(gcode);
    
    pushposition++;
    if(pushposition >= CAPACITY)
        pushposition = 0;
    
    if(pushposition == pop_position) // running out of buffer room
        return 1;
    
    return 0;
}

/* Take the first G-code object from the buffer and return it. */
GcodeInstruction GcodeStack::popBuffer() {
    GcodeInstruction* gcode = buffer[pop_position];
    GcodeInstruction gci = *gcode; // keep the data when we delete the pointer
    if(gcode == NULL) {
        gci.instruction = G_BAD;
        return gci;
    }

    delete buffer[pop_position];
    buffer[pop_position] = NULL;
    
    pop_position++;
    if(pop_position >= CAPACITY)
        pop_position = 0;

    return gci;
}

/* Look at the next instruction without removing it from the buffer */
GcodeInstruction GcodeStack::peekBuffer() {
    return *buffer[pop_position];
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
GcodeInstruction GcodeStack::parseG(int code, unsigned int cursor, String input) {
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
                        float f = atof(input.substring(cursor + 1).c_str());
                        g.argument[3] = f * PRECISION;
                        break;
                    }
                    case 'F':
                    {
                        float f = atof(input.substring(cursor + 1).c_str());
                        g.argument[4] = f * PRECISION;
                        break;
                    }
                }
            }
            break;
        case G28:
            for(; cursor<input.length(); cursor++) {
                switch(input[cursor]) {
                    case 'X':
                        g.argument[0] = 1;
                        break;
                    case 'Y':
                        g.argument[1] = 1;
                        break;
                    case 'Z':
                        g.argument[2] = 1;
                        break;                    
                }
            }
            break;
        default:
            break;
    }
    
    return g;
}

/* Parse G-code commands starting with 'M'. These are mostly RepRap specific */
GcodeInstruction GcodeStack::parseM(int code, unsigned int cursor, String input) {
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