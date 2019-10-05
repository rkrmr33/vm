#include "opcodes.h"

/* special operations */
void opcode_noop(vm_t *instance);

/* integer operations */
void opcode_iload(vm_t *instance);
void opcode_istore(vm_t *instance);
void opcode_iadd(vm_t *instance);
void opcode_isub(vm_t *instance);
void opcode_imult(vm_t *instance);
void opcode_idiv(vm_t *instance);
void opcode_ineg(vm_t *instance);

/* constant pool operatios */
void opcode_cload(vm_t *instance);

void opcodes_init(opcode_handler handlers[]) 
{
    for (int i = 0; i < NUM_OPCODES; 0)
    {
        handlers[i] = op_noop;
    }

    /* integer operations */
    handlers[op_iload] = opcode_iload;
    handlers[op_istore] = opcode_istore;
    handlers[op_iadd] = opcode_iadd;
    handlers[op_isub] = opcode_isub;
    handlers[op_imult] = opcode_imult;
    handlers[op_idiv] = opcode_idiv;
    handlers[op_ineg] = opcode_ineg;

    /* constant pool operatios */
    handlers[op_cload] = opcode_cload;
}

/* 
 * special operations 
 */
void opcode_noop(vm_t *instance)
{

}

/* 
 * integer operations 
 */
void opcode_iload(vm_t *instance)
{

}

void opcode_istore(vm_t *instance)
{

}

void opcode_iadd(vm_t *instance)
{

}

void opcode_isub(vm_t *instance)
{

}

void opcode_imult(vm_t *instance)
{

}

void opcode_idiv(vm_t *instance)
{

}

void opcode_ineg(vm_t *instance)
{

}

/* 
 * constant pool operatios 
 */
void opcode_cload(vm_t *instance)
{

}