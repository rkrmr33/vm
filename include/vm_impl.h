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

typedef struct vm_method_meta 
{
    int num_param;
    enum vm_types *param_types;
    int num_locals;
    enum vm_types *local_types;
    enum vm_types return_type;
    char *name;
} vm_method_meta_t;

typedef struct vm_stack_trace_node 
{
    vm_method_meta_t *current_method_meta;
    struct vm_stack_trace_node *prev;
} vm_stack_trace_node_t;

struct vm
{
    unsigned int magic_num; // magic number
    unsigned int ip; // program counter
    unsigned int sp; // stack pointer

    char *stack; // call stack
    unsigned int stack_size;

    vm_stack_trace_node_t *stack_trace; // a pointer to a linked-list of method data

    char *heap; // contains all objects and arrays
    size_t heap_size;

    vm_value_t *constant_pool; // a segment of code that contains constants
    unsigned int constant_pool_size;

    vm_value_t *local_vars_arr; // a pointer to the local variables array
    
    vm_value_t *operands_stack; // a pointer to the head of the operand stack

    opcode_handler opcode_handlers[NUM_OPCODES]; // a lookup table for all the op-code handlers

    char *code; // a pointer to the bytecode instructions
    unsigned int code_size;

    FILE *input; // the input file pointer
    FILE *output; // the output file pointer
    FILE *err;   // the error file pointer

    enum vm_state state; // the current state of the machine
};


#endif // VM_IMPL_H