#ifndef OPCODES_H
#define OPCODES_H

#include "vm.h" // vm_t

#define NUM_OPCODES 255

typedef void (*opcode_handler)(vm_t *instance);

void opcodes_init(opcode_handler handlers[]);

enum opcodes 
{
    /*
    * special operations
    */
    op_noop   = 0x00, // no operation
    op_print  = 0x01, // print the variable at the top of the stack

    /*
    * Integer operations
    */
    op_iload  = 0x10, // loads local integer to op stack
    op_istore = 0x11, // stores result from op stack to local integer
    op_iadd   = 0x12, // adds two integers loaded to op stack and pushes back the result
    op_isub   = 0x13, // subtracts two integers loaded to op stack and pushes back the result
    op_imult  = 0x14, // multiplies two integers loaded to op stack and pushes back the result
    op_idiv   = 0x15, // divides two integers loaded to op stack and pushes back the result
    op_ineg   = 0x16, // negates the integer at the top of the op stack

    /*
    * constant pool operations
    */
    op_cload  = 0x20, // loads a value from the const pool to the stack
};

#endif // OPCODES_H