#include "opcodes.h"

/* special operations */
void opcode_noop(vm_t *instance);
void opcode_const(vm_t *instance);
void opcode_local(vm_t *instance);

/* integer operations */
void opcode_iload(vm_t *instance);
void opcode_istore(vm_t *instance);
void opcode_iadd(vm_t *instance);
void opcode_isub(vm_t *instance);
void opcode_imult(vm_t *instance);
void opcode_idiv(vm_t *instance);
void opcode_ineg(vm_t *instance);
void opcode_iprint(vm_t *instance);
void opcode_ilocal(vm_t *instance);

/* string operations */
void opcode_sload(vm_t *instance);
void opcode_sstore(vm_t *instance);
void opcode_sprint(vm_t *instance);
void opcode_slocal(vm_t *instance);

/* constant pool operatios */
void opcode_icload(vm_t *instance);
void opcode_iconst(vm_t *instance);
void opcode_scload(vm_t *instance);
void opcode_sconst(vm_t *instance);


void opcodes_init(opcode_handler handlers[]) 
{
    for (int i = 0; i < NUM_OPCODES; ++i)
    {
        handlers[i] = opcode_noop;
    }

    /* special operations */
    handlers[OP_CONST] = opcode_const;
    handlers[OP_LOCAL] = opcode_local;

    /* integer operations */
    handlers[OP_ILOAD] = opcode_iload;
    handlers[OP_ISTORE] = opcode_istore;
    handlers[OP_IADD] = opcode_iadd;
    handlers[OP_ISUB] = opcode_isub;
    handlers[OP_IMULT] = opcode_imult;
    handlers[OP_IDIV] = opcode_idiv;
    handlers[OP_INEG] = opcode_ineg;
    handlers[OP_IPRINT] = opcode_iprint;
    handlers[OP_ILOCAL] = opcode_ilocal;

    /* string operations */
    handlers[OP_SLOAD] = opcode_sload;
    handlers[OP_SSTORE] = opcode_sstore;
    handlers[OP_SPRINT] = opcode_sprint;
    handlers[OP_SLOCAL] = opcode_slocal;

    /* constant pool operatios */
    handlers[OP_ICLOAD] = opcode_icload;
    handlers[OP_ICONST] = opcode_iconst;
    handlers[OP_SCLOAD] = opcode_scload;
    handlers[OP_SCONST] = opcode_sconst;
}

/* special operations */
void opcode_noop(vm_t *instance) 
{

}

void opcode_const(vm_t *instance) 
{

}

void opcode_local(vm_t *instance) 
{

}

/* integer operations */
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
void opcode_iprint(vm_t *instance) 
{

}
void opcode_ilocal(vm_t *instance) 
{

}

/* string operations */
void opcode_sload(vm_t *instance) 
{

}
void opcode_sstore(vm_t *instance) 
{

}
void opcode_sprint(vm_t *instance) 
{

}
void opcode_slocal(vm_t *instance) 
{

}

/* constant pool operatios */
void opcode_icload(vm_t *instance) 
{

}
void opcode_iconst(vm_t *instance) 
{

}
void opcode_scload(vm_t *instance) 
{

}
void opcode_sconst(vm_t *instance) 
{

}