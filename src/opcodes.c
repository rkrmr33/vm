#include <assert.h> /* assert */
#include <stdio.h> /* TODO: remove */

#include "vm_impl.h" /* to access vm fields  */
#include "vm_util.h" /* vm utility functions */

#include "opcodes.h"

/* special operations */
int opcode_unknown(vm_t *instance);
int opcode_noop(vm_t *instance);
int opcode_const(vm_t *instance);
int opcode_local(vm_t *instance);
int opcode_halt(vm_t *instance);
int opcode_stop(vm_t *instance);

/* integer operations */
int opcode_iload(vm_t *instance);
int opcode_istore(vm_t *instance);
int opcode_iadd(vm_t *instance);
int opcode_isub(vm_t *instance);
int opcode_imult(vm_t *instance);
int opcode_idiv(vm_t *instance);
int opcode_ineg(vm_t *instance);
int opcode_iprint(vm_t *instance);
int opcode_ilocal(vm_t *instance);

/* string operations */
int opcode_sload(vm_t *instance);
int opcode_sstore(vm_t *instance);
int opcode_sprint(vm_t *instance);
int opcode_slocal(vm_t *instance);

/* constant pool operatios */
int opcode_icload(vm_t *instance);
int opcode_iconst(vm_t *instance);
int opcode_scload(vm_t *instance);
int opcode_sconst(vm_t *instance);


void init_opcode_handlers(opcode_handler handlers[]) 
{
    for (int i = 0; i < NUM_OPCODES; ++i)
    {
        handlers[i] = opcode_unknown;
    }

    /* special operations */
    handlers[OP_NOOP] = opcode_noop;
    handlers[OP_CONST] = opcode_const;
    handlers[OP_LOCAL] = opcode_local;
    handlers[OP_HALT] = opcode_halt;
    handlers[OP_STOP] = opcode_stop;

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
int opcode_unknown(vm_t *instance)
{
    return -1;
}

int opcode_noop(vm_t *instance) 
{
    assert(instance);

    return 0;
}

int opcode_const(vm_t *instance) 
{
    assert(instance);
    // this should not be executed by the vm at run-time
    // this should only be executed at load-time...

    return -1;
}

int opcode_local(vm_t *instance) 
{
    int local_var_arr_size = 0;
    
    assert(instance);

    local_var_arr_size = read_int_value(instance);

    printf("local var arr size %d\n", local_var_arr_size);

    return 0;
}

int opcode_halt(vm_t *instance)
{
    // TODO: support HALT opcode

    return 0;
}

int opcode_stop(vm_t *instance)
{
    assert(instance);

    instance->state = VM_FINISHED;

    return 0;
}

/* integer operations */
int opcode_iload(vm_t *instance) 
{
    return 0;
}
int opcode_istore(vm_t *instance) 
{
    // TODO: support ISTORE
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
int opcode_ilocal(vm_t *instance) 
{
    vm_value_t local_int = {0};
    
    assert(instance);

    local_int.type = INTEGER_TYPE;
    local_int.value.integer_value = read_int_value(instance);

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
int opcode_slocal(vm_t *instance) 
{
    vm_value_t local_string = {0};
    int cpool_index = 0;
    
    assert(instance);

    local_string.type = STRING_TYPE;
    cpool_index = read_int_value(instance);
    // TODO: check type of value in constant pool
    local_string.value.string_value = instance->constant_pool[cpool_index].value.string_value;

    return 0;
}

/* constant pool operatios */
int opcode_icload(vm_t *instance) 
{

    return 0;
}
int opcode_iconst(vm_t *instance) 
{

    return 0;
}
int opcode_scload(vm_t *instance) 
{

    return 0;
}
int opcode_sconst(vm_t *instance) 
{

    return 0;
}