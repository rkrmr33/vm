#include <assert.h> /* assert */
#include <stdio.h> /* TODO: remove */

#include "vm_impl.h" /* to access vm fields  */
#include "vm_util.h" /* vm utility functions */

#include "opcodes.h"

/* special operations */
int opcode_unknown(vm_t *instance);
int opcode_noop(vm_t *instance);
int opcode_halt(vm_t *instance);
int opcode_stop(vm_t *instance);
int opcode_pop(vm_t *instance);
int opcode_call(vm_t *instance);
int opcode_ret(vm_t *instance);

/* integer operations */
int opcode_iload(vm_t *instance);
int opcode_istore(vm_t *instance);
int opcode_ipush(vm_t *instance);
int opcode_iadd(vm_t *instance);
int opcode_isub(vm_t *instance);
int opcode_imult(vm_t *instance);
int opcode_idiv(vm_t *instance);
int opcode_ineg(vm_t *instance);
int opcode_iprint(vm_t *instance);
int opcode_iret(vm_t *instance);

/* string operations */
int opcode_sload(vm_t *instance);
int opcode_sstore(vm_t *instance);
int opcode_sprint(vm_t *instance);
int opcode_sret(vm_t *instance);

/* constant pool operatios */
int opcode_cload(vm_t *instance);

void init_opcode_handlers(opcode_handler *handlers) 
{
    for (int i = 0; i < NUM_OPCODES; ++i)
    {
        handlers[i] = opcode_unknown;
    }

    /* special operations */
    handlers[OP_NOOP] = opcode_noop;
    handlers[OP_HALT] = opcode_halt;
    handlers[OP_STOP] = opcode_stop;
    handlers[OP_POP] = opcode_pop;
    handlers[OP_CALL] = opcode_call;
    handlers[OP_RET] = opcode_ret;

    /* integer operations */
    handlers[OP_ILOAD] = opcode_iload;
    handlers[OP_ISTORE] = opcode_istore;
    handlers[OP_IPUSH] = opcode_ipush;
    handlers[OP_IADD] = opcode_iadd;
    handlers[OP_ISUB] = opcode_isub;
    handlers[OP_IMULT] = opcode_imult;
    handlers[OP_IDIV] = opcode_idiv;
    handlers[OP_INEG] = opcode_ineg;
    handlers[OP_IPRINT] = opcode_iprint;
    handlers[OP_IRET] = opcode_iret;

    /* string operations */
    handlers[OP_SLOAD] = opcode_sload;
    handlers[OP_SSTORE] = opcode_sstore;
    handlers[OP_SPRINT] = opcode_sprint;
    handlers[OP_SRET] = opcode_sret;

    /* constant pool operatios */
    handlers[OP_CLOAD] = opcode_cload;
}

/* special operations */
int opcode_unknown(vm_t *instance)
{
    return 0;
}

int opcode_noop(vm_t *instance)
{
    return 0;
}

int opcode_halt(vm_t *instance)
{
    return 0;
}

int opcode_stop(vm_t *instance)
{
    return 0;
}

int opcode_pop(vm_t *instance)
{
    return 0;
}

int opcode_call(vm_t *instance)
{
    return 0;
}

int opcode_ret(vm_t *instance)
{
    return 0;
}


/* integer operations */
int opcode_iload(vm_t *instance)
{
    return 0;
}

int opcode_istore(vm_t *instance)
{
    return 0;
}

int opcode_ipush(vm_t *instance)
{
    return 0;
}

int opcode_iadd(vm_t *instance)
{
    return 0;
}

int opcode_isub(vm_t *instance)
{
    return 0;
}

int opcode_imult(vm_t *instance)
{
    return 0;
}

int opcode_idiv(vm_t *instance)
{
    return 0;
}

int opcode_ineg(vm_t *instance)
{
    return 0;
}

int opcode_iprint(vm_t *instance)
{
    return 0;
}

int opcode_iret(vm_t *instance)
{
    return 0;
}


/* string operations */
int opcode_sload(vm_t *instance)
{
    return 0;
}

int opcode_sstore(vm_t *instance)
{
    return 0;
}

int opcode_sprint(vm_t *instance)
{
    return 0;
}

int opcode_sret(vm_t *instance)
{
    return 0;
}

/* constant pool operations */
int opcode_cload(vm_t *instance)
{
    return 0;
}