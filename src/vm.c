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
#define MAIN_METHOD_NAME "main"

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
    free_stack_trace(instance);
    free_code(instance);

    free(instance);
    instance = NULL;
}

int vm_run(vm_t *instance)
{
    vm_instruction_t *instruction = NULL;
    int res = 0;

    assert(instance);

    if (VM_READY != instance->state)
    {
        print_error(instance, "vm is not at ready state");
    }

    instance->state = VM_RUNNING;

    while (VM_RUNNING == instance->state)
    {
        instruction = read_next_instruction(instance);
        res = instance->opcode_handlers[instruction->opcode](instance);
    }

    return 0;
}


/* STATIC FUNCTIONS */
static int build_constant_pool(vm_t *instance)
{
    char cur_opcode = 0;
    int cur_type = 0;
    vm_value_t *cur_value = NULL;
    vm_method_meta_t *cur_method = NULL;
    int str_len = 0;
    int size = 0;

    assert(instance);

    instance->constant_pool_size = read_byte_value(instance);
    printf("cpool size: %d\n", instance->constant_pool_size);

    instance->constant_pool = (vm_value_t *)malloc(instance->constant_pool_size * sizeof(vm_value_t));
    if (NULL == instance->constant_pool)
    {
        return -1;
    }

    for (int i = 0; i < instance->constant_pool_size; ++i)
    {
        cur_type = read_byte_value(instance);
        cur_value = &instance->constant_pool[i];
        
        switch (cur_type)
        {
            case VM_TYPE_BYTE:
                cur_value->type = VM_TYPE_BYTE;
                cur_value->value.byte_value = read_byte_value(instance);
                break;
            case VM_TYPE_INTEGER:
                cur_value->type = VM_TYPE_INTEGER;
                cur_value->value.integer_value = read_int_value(instance);
                break;
            case VM_TYPE_FLOAT:
                cur_value->type = VM_TYPE_FLOAT;
                // TODO: add support for float
                break;
            case VM_TYPE_LONG:
                cur_value->type = VM_TYPE_LONG;
                // TODO: add support for long
                break;
            case VM_TYPE_DOUBLE:
                cur_value->type = VM_TYPE_DOUBLE;
                // TODO: add support for double
                break;
            case VM_TYPE_STRING:
                cur_value->type = VM_TYPE_STRING;
                cur_value->value.string_value = read_string_value(instance);
                if (NULL == cur_value->value.string_value)
                {
                    return -1;
                }
                break;
            case VM_TYPE_REFERENCE:
                cur_value->type = VM_TYPE_REFERENCE;
                // TODO: add support for reference types
                break;
            case VM_TYPE_METHOD:
                cur_value->type = VM_TYPE_METHOD;
                cur_method = (vm_method_meta_t *)malloc(sizeof(vm_method_meta_t));
                if (NULL == cur_method) 
                {
                    return -1;
                }

                cur_method->name = read_string_value(instance);
                if (NULL == cur_method->name)
                {
                    return -1;
                }

                cur_method->return_type = read_byte_value(instance);

                cur_method->num_locals = read_byte_value(instance);
                cur_method->local_types = (enum vm_types *)malloc(sizeof(int) * cur_method->num_locals);
                if (NULL == cur_method->local_types) 
                {
                    return -1;
                }
                for (int i = 0; i < cur_method->num_locals; ++i)
                {
                    cur_method->local_types[i] = read_byte_value(instance);
                }

                cur_method->num_params = read_byte_value(instance);
                cur_method->param_types = (enum vm_types *)malloc(sizeof(int) * cur_method->num_params);
                if (NULL == cur_method->param_types) 
                {
                    return -1;
                }
                for (int i = 0; i < cur_method->num_params; ++i)
                {
                    cur_method->param_types[i] = read_byte_value(instance);
                }

                cur_method->offset = read_int_value(instance);

                cur_value->value.method_value = cur_method;

                if (-1 == check_main_method(instance, MAIN_METHOD_NAME, cur_method))
                {
                    return -1;
                }

                break;
        }

        print_vm_value(cur_value);
    }

    // move to point to first instruction
    instance->instructions = (vm_instruction_t *)&instance->code[instance->ip];
    // reset to read instructions
    instance->ip = 0; 
    
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

    instance->heap = (char *)malloc(sizeof(char) * instance->heap_size);
    if (NULL == instance->heap)
    {
        return -1;
    }

    instance->stack = (vm_value_t *)malloc(sizeof(char) * instance->stack_size);
    if (NULL == instance->stack)
    {
        return -1;
    }

    instance->stack_trace = NULL;
    instance->instructions = NULL;

    instance->sp = 0;
    instance->ip = 0;
    instance->lap = 0;
    instance->osp = 0;

    init_opcode_handlers(instance->opcode_handlers);

    instance->output = (NULL == output ? DEFAULT_OUTPUT : output);
    instance->input = (NULL == input ? DEFAULT_INPUT : input);
    instance->err = (NULL == err ? DEFAULT_ERR : err);

    return 0;
}

