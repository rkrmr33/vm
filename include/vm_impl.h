#ifndef VM_IMPL_H
#define VM_IMPL_H

#include "opcodes.h" /* opcode_handler */

enum vm_types
{
    INTEGER_TYPE   = 0x01,
    FLOAT_TYPE     = 0x02,
    STRING_TYPE    = 0x03,
    REFERENCE_TYPE = 0x04
};

typedef struct vm_value
{
    enum vm_types type;
    union
    {
        int integer_value;
        float float_value;
        char *string_value;
        void *reference_value;
    } value;
} vm_value_t;

struct vm
{
    unsigned int magic_num; // magic number
    unsigned int ip; // program counter
    unsigned int sp; // stack pointer

    char *stack; // call stack
    unsigned int stack_size;

    char *heap; // contains all objects and arrays
    size_t heap_size;

    vm_value_t *constant_pool; // a segment of code that contains constants
    unsigned int constant_pool_size;

    vm_value_t *local_vars_arr; // a pointer to the local variables array
    vm_value_t *operands_stack; // a pointer to the head of the operand stack

    opcode_handler opcode_handlers[NUM_OPCODES]; // a lookup table for all the op-code handlers

    char *code; // a pointer to the bytecode instructions
    unsigned int code_size;

    err_handler error_handler; // error handler

    enum vm_state state; // the current state of the machine
};


#endif // VM_IMPL_H