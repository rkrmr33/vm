#include <assert.h>    /* assert    */
#include <stdio.h>     /* perror    */
#include <stdlib.h>    /* malloc    */
#include <string.h>    /* strlen    */

#include "opcodes.h"   /* opcodes */
#include "vm_impl.h"   /* private vm header */
#include "vm_util.h"   /* utility functions */

#include "vm.h"        /* public vm header */

#define DEFAULT_ERR_HANDLER default_err_handler
#define MAGIC_NUM 0xBABEFACE
#define DEFAULT_HEAP_SIZE 1000000 // 1mb
#define DEFAULT_STACK_SIZE 100000 // 100kb 

#define DEFAULT_OUTPUT stdout
#define DEFAULT_INPUT stdin
#define DEFAULT_ERR stderr

static int init_vm_fields(vm_t *instance, 
                          unsigned int stack_size,
                          size_t heap_size,
                          FILE *input, 
                          FILE *output, 
                          FILE *err);
static int build_constant_pool(vm_t *instance);

vm_t *vm_create(const char *file_path,
                unsigned int stack_size,
                size_t heap_size,
                FILE *output,
                FILE *input,
                FILE *err)
{
    int res = 0;
    vm_t *new_instance = NULL;

    assert(NULL != file_path);
    
    new_instance = (vm_t *)malloc(sizeof(vm_t));
    if (NULL == new_instance)
    {
        return NULL;
    }

    res = init_vm_fields(new_instance, stack_size, heap_size, output, input, err);
    if (0 != res)
    {
        vm_free(new_instance);

        return NULL;
    }

    res = load_bytecode_from_file(file_path, new_instance);
    if (0 != res)
    {
        vm_free(new_instance);

        return NULL;
    }

    res = validate_magic_number(new_instance);
    if (0 != res)
    {
        print_error(new_instance, "file with wrong magic number!");
        print_error(new_instance, file_path);
        vm_free(new_instance);

        return NULL;
    }

    res = build_constant_pool(new_instance);
    if (0 != res) 
    {
        vm_free(new_instance);

        return NULL;
    }

    new_instance->state = VM_READY;

    return new_instance;
}

void vm_free(vm_t *instance)
{   
    assert(instance);

    free_heap(instance);
    free_stack(instance);
    free_constant_pool(instance);
    free_code(instance);

    free(instance);
    instance = NULL;
}

int vm_run(vm_t *instance)
{
    int cur_opcode = 0;
    int res = 0;

    assert(instance);

    if (VM_READY != instance->state)
    {
        print_error(instance, "vm is not at ready state");
    }

    instance->state = VM_RUNNING;

    while (VM_RUNNING == instance->state)
    {
        cur_opcode = read_opcode(instance);
        res = instance->opcode_handlers[cur_opcode](instance);
    }

    return 0;
}


/* STATIC FUNCTIONS */
static int build_constant_pool(vm_t *instance)
{
    char cur_opcode = 0;
    vm_value_t *cur_value = NULL;
    int size = 0;

    assert(instance);

    cur_opcode = read_opcode(instance);

    if (OP_CONST != cur_opcode) {
        return -1;
    }

    instance->constant_pool_size = read_int_value(instance);

    instance->constant_pool = (vm_value_t *)malloc(instance->constant_pool_size * sizeof(vm_value_t));
    if (NULL == instance->constant_pool)
    {
        return -1;
    }

    for (int i = 0; i < instance->constant_pool_size; ++i)
    {
        cur_opcode = read_opcode(instance);
        cur_value = &(instance->constant_pool[i]);
        
        switch (cur_opcode)
        {
            case OP_SCONST: // STRING_TYPE constant
                cur_value->type = STRING_TYPE;
                cur_value->value.string_value = read_string_value(instance);
                if (NULL == cur_value->value.string_value)
                {
                    return -1;
                }
                break;
            case OP_ICONST: // INTEGER_TYPE constant
                cur_value->type = INTEGER_TYPE;
                cur_value->value.integer_value = read_int_value(instance);
                break;
            default:
                print_error(instance, "constant_pool_load: unknown type");
                return -1;
        }
    }
    
    return 0;
}

static int init_vm_fields(vm_t *instance, 
                          unsigned int stack_size,
                          size_t heap_size,
                          FILE *input, 
                          FILE *output, 
                          FILE *err)
{
    assert(instance);

    memset(instance, 0, sizeof(vm_t));

    instance->state = VM_INIT;
    instance->magic_num = MAGIC_NUM;
    instance->heap_size = (0 == heap_size ? DEFAULT_HEAP_SIZE : heap_size);
    instance->stack_size = (0 == stack_size ? DEFAULT_STACK_SIZE : stack_size);

    instance->heap = malloc(sizeof(char) * instance->heap_size);
    if (NULL == instance->heap)
    {
        return -1;
    }

    instance->stack = malloc(sizeof(char) * instance->stack_size);
    if (NULL == instance->stack)
    {
        return -1;
    }

    instance->local_vars_arr = (vm_value_t *)instance->stack;
    instance->operands_stack = (vm_value_t *)instance->stack + 1;
    init_opcode_handlers(instance->opcode_handlers);

    instance->output = (NULL == output ? DEFAULT_OUTPUT : output);
    instance->input = (NULL == input ? DEFAULT_INPUT : input);
    instance->err = (NULL == err ? DEFAULT_ERR : err);

    return 0;
}

