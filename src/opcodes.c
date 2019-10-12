#include <assert.h> /* assert */
#include <stdio.h> /* TODO: remove */
#include <string.h> /* memcpy */

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
    int index = 0, res = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->stack && instance->constant_pool);

    index = get_instruction_arg(instance);

    if (index >= instance->constant_pool_size)
    {
        fprintf(instance->err, "[call] failed, index %d is out of constant pool bounds!\n",
            index);

        return -1;
    }

    value = &instance->constant_pool[index];

    if (VM_TYPE_METHOD != value->type)
    {
        fprintf(instance->err, "[call] failed, constant is of type: %s!\n",
            get_type_name(value->type));

        return -1;
    }

    res = open_stack_frame(instance, value->value.method_value);
    if (0 != res)
    {  
        fprintf(instance->err, "[call] failed, could not open stack frame for method: %s!\n",
            value->value.method_value->name);

        return -1;
    }

    instance->ip = value->value.method_value->offset;

    return 0;
}

int opcode_ret(vm_t *instance)
{
    assert(instance && instance->stack_trace);

    pop_stack_frame(instance);
    
    return 0;
}

/* integer operations */
int opcode_iload(vm_t *instance)
{
    int index = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    index = get_instruction_arg(instance);
    value = &instance->stack[instance->lap + index];

    if (VM_TYPE_INTEGER != value->type)
    {
        fprintf(instance->err, "[iload] failed, local variable %d is of type: %s\n",
            index, get_type_name(value->type));

        return -1;
    }

    // TODO: check for stack overflow
    instance->stack[instance->osp].type = VM_TYPE_INTEGER;
    instance->stack[instance->osp].value.integer_value = value->value.integer_value;
    ++instance->osp;

    return 0;
}

int opcode_istore(vm_t *instance)
{
    int intVal = 0;
    int arg = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    arg = get_instruction_arg(instance);

    if (get_operand_stack_size(instance) <= 0)
    {
        fprintf(instance->err, "[istore] failed, operand stack is empty!\n");

        return -1;
    }

    value = &instance->stack[instance->osp - 1];

    if (VM_TYPE_INTEGER != value->type)
    {
        fprintf(instance->err, "[istore] failed, operand stack top is of type: %s\n",
            get_type_name(value->type));

        return -1;
    }

    if (VM_TYPE_INTEGER != instance->stack[instance->lap + arg].type)
    {
        fprintf(instance->err, "[istore] failed, trying to store int to local "
            "variable of type: %s\n",
            get_type_name(instance->stack[instance->lap + arg].type));

        return -1;
    }

    instance->stack[instance->lap + arg].value.integer_value = value->value.integer_value;
    --instance->osp;

    return 0;
}

int opcode_ipush(vm_t *instance)
{
    int intVal = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    // TODO: check stack overflow

    intVal = get_instruction_arg(instance);
    value = &instance->stack[instance->osp];
    value->type = VM_TYPE_INTEGER;
    value->value.integer_value = intVal;
    ++instance->osp;

    return 0;
}

int opcode_iadd(vm_t *instance)
{
    vm_value_t *op1 = NULL, *op2 = NULL;

    assert(instance && instance->stack);

    if (get_operand_stack_size(instance) < 2)
    {
        fprintf(instance->err, "[iadd] failed, operand stack does not have enough operands\n");

        return -1;
    }

    op1 = &instance->stack[instance->osp - 2];
    op2 = &instance->stack[instance->osp - 1];

    if (VM_TYPE_INTEGER != op1->type)
    {
        fprintf(instance->err, "[iadd] failed, operand1 is of type: %s\n",
            get_type_name(op1->type));

        return -1;
    }

    if (VM_TYPE_INTEGER != op2->type)
    {
        fprintf(instance->err, "[iadd] failed, operand2 is of type: %s\n",
            get_type_name(op2->type));

        return -1;
    }
    
    op1->value.integer_value += op2->value.integer_value;
    --instance->osp;

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
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    if (get_operand_stack_size(instance) <= 0)
    {
        fprintf(instance->err, "[iprint] failed, operand stack is empty!\n");

        return -1;
    }

    value = &instance->stack[instance->osp - 1];
    if (VM_TYPE_INTEGER != value->type)
    {
        fprintf(instance->err, "[iprint] failed, operand stack top is of type: %s\n",
            get_type_name(value->type));

        return -1;
    }
    --instance->osp;

    fprintf(instance->output, "%d\n", value->value.integer_value);
    fflush(instance->output);

    return 0;
}

int opcode_iret(vm_t *instance)
{
    vm_value_t *result = NULL;
    int prev_osp = 0, prev_lap = 0, prev_sp = 0;
    int prev_num_locals = 0, prev_num_params = 0;

    assert(instance && instance->stack);

    if (get_operand_stack_size(instance) <= 0)
    {
        fprintf(instance->err, "[iret] failed, operand stack is empty\n");

        return -1;
    }

    result = &instance->stack[instance->osp - 1];

    if (VM_TYPE_INTEGER != result->type)
    {
        fprintf(instance->err, "[iret] failed, result is of type: %s\n",
            get_type_name(result->type));

        return -1;
    }

    pop_stack_frame(instance);

    memcpy(&instance->stack[instance->osp], result, sizeof(vm_value_t));
    ++instance->osp;

    return 0;
}


/* string operations */
int opcode_sload(vm_t *instance)
{
    int index = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    index = get_instruction_arg(instance);
    value = &instance->stack[instance->lap + index];

    if (VM_TYPE_STRING != value->type)
    {
        fprintf(instance->err, "[sload] failed, local variable %d is of type: %s\n",
            index, get_type_name(value->type));

        return -1;
    }

    // TODO: check for stack overflow
    instance->stack[instance->osp].type = VM_TYPE_STRING;
    instance->stack[instance->osp].value.string_value = value->value.string_value;
    ++instance->osp;

    return 0;
}

int opcode_sstore(vm_t *instance)
{
    char *stringVal = 0;
    int arg = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    arg = get_instruction_arg(instance);

    if (get_operand_stack_size(instance) <= 0)
    {
        fprintf(instance->err, "[sstore] failed, operand stack is empty!\n");

        return -1;
    }

    value = &instance->stack[instance->osp - 1];

    if (VM_TYPE_STRING != value->type)
    {
        fprintf(instance->err, "[sstore] failed, operand stack top is of type: %s\n",
            get_type_name(value->type));

        return -1;
    }

    if (VM_TYPE_STRING != instance->stack[instance->lap + arg].type)
    {
        fprintf(instance->err, "[sstore] failed, trying to store string to local "
            "variable of type: %s\n",
            get_type_name(instance->stack[instance->lap + arg].type));

        return -1;
    }

    instance->stack[instance->lap + arg].value.string_value = value->value.string_value;
    --instance->osp;

    return 0;
}

int opcode_sprint(vm_t *instance)
{
    vm_value_t *value = NULL;

    assert(instance && instance->stack);

    if (get_operand_stack_size(instance) <= 0)
    {
        fprintf(instance->err, "[sprint] failed, operand stack is empty!\n");

        return -1;
    }

    value = &instance->stack[instance->osp - 1];
    if (VM_TYPE_STRING != value->type)
    {
        fprintf(instance->err, "[sprint] failed, operand stack top is of type: %s\n",
            get_type_name(value->type));

        return -1;
    }
    --instance->osp;

    fprintf(instance->output, "%s\n", value->value.string_value);
    fflush(instance->output);

    return 0;
}

int opcode_sret(vm_t *instance)
{
    return 0;
}

/* constant pool operations */
int opcode_cload(vm_t *instance)
{
    int index = 0;
    vm_value_t *value = NULL;

    assert(instance && instance->constant_pool && instance->stack);

    index = get_instruction_arg(instance);

    if (index >= instance->constant_pool_size)
    {
        fprintf(instance->err, "[cload] failed, index %d is out of constant pool bounds!\n",
            index);

        return -1;
    }

    // TODO: check stack overflow
    memcpy(&instance->stack[instance->osp],
           &instance->constant_pool[index],
           sizeof(vm_value_t));

    ++instance->osp;

    return 0;
}