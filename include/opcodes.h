#ifndef OPCODES_H
#define OPCODES_H

#include "vm.h" // vm_t

#define NUM_OPCODES 255

typedef int (*opcode_handler)(vm_t *instance);

void init_opcode_handlers(opcode_handler *handlers);

enum opcodes 
{
    /*
    * special operations
    */
    OP_NOOP   = 0x00, // no operation
    OP_CONST  = 0x01, // declares following constant pool size
    OP_LOCAL  = 0x02, // declares following local variables array size
    OP_HALT   = 0x03, // halts the machine for a variable amount of seconds
    OP_STOP   = 0x04, // stops the machine changing its state to FINISHED

    /*
    * Integer operations
    */
    OP_ILOAD  = 0x10, // loads local integer to op stack
    OP_ISTORE = 0x11, // stores result from op stack to local integer
    OP_IADD   = 0x12, // adds two integers loaded to op stack and pushes back the result
    OP_ISUB   = 0x13, // subtracts two integers loaded to op stack and pushes back the result
    OP_IMULT  = 0x14, // multiplies two integers loaded to op stack and pushes back the result
    OP_IDIV   = 0x15, // divides two integers loaded to op stack and pushes back the result
    OP_INEG   = 0x16, // negates the integer at the top of the op stack
    OP_IPRINT = 0x17, // print the integer at the top of the op stack
    OP_ILOCAL = 0x18, // declares a local variable of type integer

    /*
    * string operations
    */
    OP_SLOAD  = 0x30, // loads a string to the op stack
    OP_SSTORE = 0x31, // stores result from op stack to local string
    OP_SPRINT = 0x33, // print the integer at the top of the op stack
    OP_SLOCAL = 0x34, // declares a local variable of type string

    /*
    * constant pool operations
    */
    OP_ICLOAD = 0x50, // loads an integer value from the const pool to the stack
    OP_ICONST = 0x51, // declares a constant integer in the constant pool
    OP_SCLOAD = 0x52, // loads an string value from the const pool to the stack
    OP_SCONST = 0x53, // declares a constant string in the constant pool
};

#endif // OPCODES_H