/* 
 * File:   gcodeinterpreter.hpp
 * Declarations for the G-code interpreter.
 * 
 */

#ifndef GCODEINTERPRETER_HPP
#define	GCODEINTERPRETER_HPP

// the numbers here are arbitrary; the important thing is that they're unique
#define G0  0
#define G1  1
#define G2  2
#define G3  3
#define G4  4
#define G10 10
#define G20 20
#define G21 21
#define G28 28
#define G29 29
#define G30 30
#define G31 31
#define G32 32
#define G90 90
#define G91 91
#define G92 92

#define M0      101
#define M1      102
#define M112    212

#define G_OK     "ok"
#define G_RESEND "rs"
#define G_ERROR  "!!"
#define G_BAD   -1

#define CAPACITY 10

class GcodeInstruction {
public:
    GcodeInstruction();
    bool cs_valid;
    int instruction;
    long int argument[8]; // arguments/options for the command
};

class GcodeStack {
public:
    GcodeStack();
    int pushAndParse(String);
    int pushBuffer(GcodeInstruction);
    GcodeInstruction popBuffer();
    GcodeInstruction peekBuffer();
    byte checkSum();
    int getLine();
protected:
    GcodeInstruction parseString(String);
    int parseCommand(unsigned int&, String);
    GcodeInstruction parseG(int, unsigned int, String);
    GcodeInstruction parseM(int, unsigned int, String);
private:
    int line;
    
    // make our own circular buffer
    GcodeInstruction *buffer[CAPACITY];
    byte pushposition;
    byte pop_position;
};

#endif	/* GCODEINTERPRETER_HPP */

