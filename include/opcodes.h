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
    OP_HALT   = 0x01, // halts the machine for a variable amount of milisec
    OP_STOP   = 0x02, // stops the machine changing its state to FINISHED
    OP_POP    = 0x03, // pops the top of the operand stack
    OP_CALL   = 0x04, // calls the method in the index at the constant pool
    OP_RET    = 0x05, // return to the calling method (void)

    /*
    * Integer operations
    */
    OP_ILOAD  = 0x10, // loads local integer to op stack
    OP_ISTORE = 0x11, // stores result from op stack to local integer
    OP_IPUSH  = 0x12,  // pushes an integer value to the op stack
    OP_IADD   = 0x13, // adds two integers loaded to op stack and pushes back the result
    OP_ISUB   = 0x14, // subtracts two integers loaded to op stack and pushes back the result
    OP_IMULT  = 0x15, // multiplies two integers loaded to op stack and pushes back the result
    OP_IDIV   = 0x16, // divides two integers loaded to op stack and pushes back the result
    OP_INEG   = 0x17, // negates the integer at the top of the op stack
    OP_IPRINT = 0x18, // print the integer at the top of the op stack
    OP_IRET   = 0x19, // returns an integer to the calling method

    /*
    * string operations
    */
    OP_SLOAD  = 0x30, // loads a string to the op stack
    OP_SSTORE = 0x31, // stores result from op stack to local string
    OP_SPRINT = 0x32, // print the integer at the top of the op stack
    OP_SRET = 0x33, // returns a string to the calling method

    /*
    * constant pool operations
    */
    OP_CLOAD = 0x50, // loads a constant from the constant pool
};

typedef struct vm_instruction 
{
    enum opcodes opcode;
    int arg;
} vm_instruction_t;

#endif // OPCODES_H