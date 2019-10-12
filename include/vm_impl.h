#ifndef VM_IMPL_H
#define VM_IMPL_H

#include "opcodes.h" /* opcode_handler */

enum vm_types
{
    VM_TYPE_BYTE      = 0x01,
    VM_TYPE_INTEGER   = 0x02,
    VM_TYPE_FLOAT     = 0x03,
    VM_TYPE_LONG      = 0x04,
    VM_TYPE_DOUBLE    = 0x05,
    VM_TYPE_STRING    = 0x06,
    VM_TYPE_REFERENCE = 0x07,
    VM_TYPE_METHOD    = 0x08,
};

typedef struct vm_method_meta 
{
    char *name;
    enum vm_types return_type;
    int num_locals;
    enum vm_types *local_types;
    int num_param;
    enum vm_types *param_types;
    unsigned int offset;
} vm_method_meta_t;

typedef struct vm_value
{
    enum vm_types type;
    union
    {
        char byte_value;
        int integer_value;
        float float_value;
        long long_value;
        double double_value;
        char *string_value;
        void *reference_value;
        vm_method_meta_t *method_value;
    } value;
} vm_value_t;

typedef struct vm_stack_trace 
{
    vm_method_meta_t *current_method_meta;
    struct vm_stack_trace *prev;
} vm_stack_trace_t;

struct vm
{
    unsigned int magic_num; // magic number
    unsigned int ip; // program counter
    unsigned int sp; // stack pointer
    unsigned int lap; // points to the local variable array of the current stack frame
    unsigned int osp; // points to the top of the operand stack of the current stack frame

    vm_value_t *stack; // call stack
    unsigned int stack_size;

    vm_stack_trace_t *stack_trace; // a pointer to a linked-list of method data

    char *heap; // contains all objects and arrays
    size_t heap_size;

    vm_value_t *constant_pool; // a segment of code that contains constants
    unsigned int constant_pool_size;

    opcode_handler opcode_handlers[NUM_OPCODES]; // a lookup table for all the op-code handlers

    vm_instruction_t *code; // a pointer to the bytecode instructions
    unsigned int code_size;

    FILE *input; // the input file pointer
    FILE *output; // the output file pointer
    FILE *err;   // the error file pointer

    enum vm_state state; // the current state of the machine
};


#endif // VM_IMPL_H